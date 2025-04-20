// SPDX-License-Identifier: BSL-1.0

#include <vector>
#include <utility>
#include <memory>

#include "traeger/format/Format.hpp"
#include "traeger/actor/Result.hpp"
#include "traeger/actor/Scheduler.hpp"
#include "traeger/actor/Promise.hpp"
#include "traeger/socket/Subscriber.hpp"

namespace
{
    using namespace traeger;

    struct listen_closure
    {
        const Scheduler scheduler;
        const Subscriber::Callback callback;
        const Promise promise;
        const Socket subscriber;

        listen_closure(const Scheduler &scheduler,
                       Subscriber::Callback &&callback,
                       const Promise &promise,
                       const Socket &subscriber) noexcept
            : scheduler(scheduler),
              callback(std::move(callback)),
              promise(promise),
              subscriber(subscriber)
        {
        }
    };

    auto listen(const List &messages,
                const std::shared_ptr<listen_closure> &closure) -> Result
    {
        String topic, format_name, encoded_message;
        auto [unpack_ok, unpack_error] = messages.get(topic, format_name, encoded_message);
        if (!unpack_ok)
        {
            return {Error{"unpacking frames: " + unpack_error}};
        }

        const auto *format = Format::by_name(format_name);
        if (!format)
        {
            return {Error{"no such format " + format_name}};
        }

        auto &&[decoded, decoded_error] = format->decode(encoded_message);
        if (!decoded)
        {
            return {Error{std::move(decoded_error)}};
        }

        closure->callback(std::move(topic), std::move(decoded).value());
        return {Value{nullptr}};
    }

    auto schedule_listen(const std::shared_ptr<listen_closure> &listen_closure) noexcept -> void
    {
        if (listen_closure->promise.has_result())
        {
            return;
        }
        const auto recv_promise = listen_closure->subscriber.recv(listen_closure->scheduler);
        recv_promise.set_promise(listen_closure->promise);
        recv_promise.then(
            [listen_closure](const Value &value) -> Result
            {
                if (auto optional = value.get_list(); optional)
                {
                    listen_closure->scheduler.schedule(
                        [listen_closure]
                        { schedule_listen(listen_closure); });
                    return listen(optional.value(), listen_closure);
                }
                else
                {
                    return {};
                }
            });
        recv_promise.fail(
            [listen_closure](const Error &error)
            {
                listen_closure->promise.set_result(error);
            });
    }
}

namespace traeger
{
    Subscriber::Subscriber(Socket &&socket) noexcept
        : subscriber_(std::move(socket))
    {
    }

    auto Subscriber::listen(const Scheduler &scheduler,
                            Callback &&callback) const noexcept -> Promise
    {
        Promise promise{scheduler};
        schedule_listen(std::make_shared<listen_closure>(scheduler, std::move(callback), promise, subscriber_));
        return promise;
    }
}
