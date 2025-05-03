// SPDX-License-Identifier: BSL-1.0

#include <utility>
#include <memory>

#include "traeger/format/Format.hpp"
#include "traeger/actor/Result.hpp"
#include "traeger/actor/Scheduler.hpp"
#include "traeger/actor/Promise.hpp"
#include "traeger/actor/Mailbox.hpp"
#include "traeger/socket/Replier.hpp"

namespace
{
    using namespace traeger;

    struct reply_closure
    {
        const Scheduler scheduler;
        const Mailbox mailbox;
        const Promise promise;
        const Socket router;

        reply_closure(const Scheduler &scheduler,
                      const Mailbox &mailbox,
                      const Promise &promise,
                      const Socket &router) noexcept
            : scheduler(scheduler),
              mailbox(mailbox),
              promise(promise),
              router(router)
        {
        }
    };

    auto reply(const List &messages,
               const std::shared_ptr<reply_closure> &closure) -> Result
    {
        String id, message_name, format_name, request;
        auto [unpack_ok, unpack_error] = messages.unpack(id, message_name, format_name, request);
        if (!unpack_ok)
        {
            return {Error{"unpacking frames: " + unpack_error}};
        }

        auto format = Format::by_name(format_name);
        if (!format)
        {
            return {Error{"no such format " + format_name}};
        }

        auto &&[decoded, decoded_error] = format->decode(request);
        if (!decoded)
        {
            return {Error{std::move(decoded_error)}};
        }

        auto maybe_arguments = decoded.value().get_list();
        if (!maybe_arguments)
        {
            return {Error{"expected a list with arguments"}};
        }
        auto arguments = maybe_arguments.value();

        closure->mailbox
            .send(closure->scheduler, message_name, arguments)
            .then(
                [format, closure, id](const Value &value) -> Result
                {
                    auto [encoded, encoded_error] = format->encode(value);
                    if (!encoded)
                    {
                        return {Error{std::move(encoded_error)}};
                    }
                    closure->router.send(closure->scheduler, {id, std::move(encoded).value(), Error{}});
                    return {Value{nullptr}};
                })
            .fail(
                [closure, id](const Error &error)
                {
                    closure->router.send(closure->scheduler, {id, String{}, Error{error}});
                });

        return {Value{nullptr}};
    }

    auto schedule_reply(const std::shared_ptr<reply_closure> &reply_closure) noexcept -> void
    {
        if (reply_closure->promise.has_result())
        {
            return;
        }

        const auto recv_promise = reply_closure->router.recv(reply_closure->scheduler);
        recv_promise.set_promise(reply_closure->promise);
        recv_promise.then(
            [reply_closure](const Value &value) -> Result
            {
                if (auto optional = value.get_list(); value.get_list())
                {
                    reply_closure->scheduler.schedule(
                        [reply_closure]
                        { schedule_reply(reply_closure); });
                    return reply(optional.value(), reply_closure);
                }
                else
                {
                    return {};
                }
            });
        recv_promise.fail(
            [reply_closure](const Error &error)
            {
                reply_closure->promise.set_result(error);
            });
    }
}

namespace traeger
{
    Replier::Replier(Socket &&socket) noexcept
        : router_(std::move(socket))
    {
    }

    auto Replier::reply(const Scheduler &scheduler,
                        const Mailbox &mailbox) const noexcept -> Promise
    {
        Promise promise{scheduler};
        schedule_reply(std::make_shared<reply_closure>(scheduler, mailbox, promise, router_));
        return promise;
    }
}
