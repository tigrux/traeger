// SPDX-License-Identifier: BSL-1.0

#include <cstddef>
#include <utility>
#include <memory>
#include <iostream>

#include <zmq.hpp>

#include "traeger/format/Format.hpp"
#include "traeger/actor/Actor.hpp"
#include "traeger/socket/Socket.hpp"
#include "traeger/socket/Replier.hpp"
#include "traeger/socket/Requester.hpp"
#include "traeger/socket/Publisher.hpp"
#include "traeger/socket/Subscriber.hpp"
#include "traeger/socket/Context.hpp"

namespace traeger
{
    struct Context::impl_type
        : public std::enable_shared_from_this<impl_type>
    {
        impl_type() noexcept
            : context_()
        {
        }

        auto socket(zmq::socket_t &&socket) noexcept -> Socket
        {
            auto actor = make_actor<socket_state>(shared_from_this(), std::move(socket));
            actor.define("recv", &socket_state::recv);
            actor.define("send", &socket_state::send);
            return Socket{actor.mailbox()};
        }

        auto context() noexcept -> zmq::context_t &
        {
            return context_;
        }

    private:
        struct socket_state
        {
            socket_state(std::shared_ptr<impl_type> &&impl,
                         zmq::socket_t &&socket) noexcept
                : impl_(std::move(impl)),
                  socket_(std::move(socket))
            {
            }

            auto recv() -> Value
            {
                List messages;
                while (true)
                {
                    zmq::message_t message;
                    if (!socket_.recv(message, zmq::recv_flags::dontwait))
                    {
                        return Value{nullptr};
                    }
                    messages.append(message.to_string());
                    if (!message.more())
                    {
                        break;
                    }
                }
                return Value{std::move(messages)};
            }

            auto send(List messages) -> Value
            {
                const UInt message_count = messages.size();
                UInt message_index = 0;
                for (const Value &value : messages)
                {
                    auto flags = message_index != message_count - 1
                                     ? zmq::send_flags::dontwait | zmq::send_flags::sndmore
                                     : zmq::send_flags::dontwait;
                    const auto &message = *value.get_string();
                    if (!socket_.send(zmq::buffer(message), flags))
                    {
                        return Value{nullptr};
                    }
                    ++message_index;
                }
                return Value{message_count};
            }

            std::shared_ptr<impl_type> impl_;
            zmq::socket_t socket_;
        };

        zmq::context_t context_;
    };

    Context::Context() noexcept
        : impl_(std::make_shared<impl_type>())
    {
    }

#if defined(__ZMQ_HPP_INCLUDED__)
    Context::operator zmq::context_t &() const
    {
        return impl_->context();
    }
#endif

    auto Context::replier(const char *address) const noexcept -> std::pair<std::optional<Replier>, String>
    {
        try
        {
            auto socket = zmq::socket_t(impl_->context(), zmq::socket_type::router);
            socket.bind(address);
            return {Replier{impl_->socket(std::move(socket))}, String{}};
        }
        catch (const std::exception &e)
        {
            return {std::nullopt, e.what()};
        }
    }

    auto Context::requester(const char *address,
                            const Format &format) const noexcept -> std::pair<std::optional<Requester>, String>
    {
        try
        {
            auto socket = zmq::socket_t(impl_->context(), zmq::socket_type::dealer);
            socket.connect(address);
            return {Requester{impl_->socket(std::move(socket)), format}, String{}};
        }
        catch (const std::exception &e)
        {
            return {std::nullopt, e.what()};
        }
    }

    auto Context::publisher(const char *address,
                            const Format &format) const noexcept -> std::pair<std::optional<Publisher>, String>
    {
        try
        {
            auto socket = zmq::socket_t(impl_->context(), zmq::socket_type::pub);
            socket.bind(address);
            return {Publisher{impl_->socket(std::move(socket)), format}, String{}};
        }
        catch (const std::exception &e)
        {
            return {std::nullopt, e.what()};
        }
    }

    auto Context::subscriber(const char *address,
                             const std::vector<String> &topics) const noexcept -> std::pair<std::optional<Subscriber>, String>
    {
        try
        {
            auto socket = zmq::socket_t(impl_->context(), zmq::socket_type::sub);
            socket.connect(address);
            for (const auto &topic : topics)
            {
                socket.set(zmq::sockopt::subscribe, topic);
            }
            return {Subscriber{impl_->socket(std::move(socket))}, String{}};
        }
        catch (const std::exception &e)
        {
            return {std::nullopt, e.what()};
        }
    }
}
