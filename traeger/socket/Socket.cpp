// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <memory>
#include <utility>
#include <vector>

#include "traeger/actor/Result.hpp"
#include "traeger/actor/Scheduler.hpp"
#include "traeger/actor/Promise.hpp"
#include "traeger/socket/Context.hpp"
#include "traeger/socket/Socket.hpp"

namespace
{
    using namespace traeger;

    struct socket_closure
    {
        const Scheduler scheduler;
        const Mailbox mailbox;
        const Promise promise;
        const List arguments;

        socket_closure(const Scheduler &scheduler,
                       const Mailbox &mailbox,
                       const Promise &promise,
                       List &&arguments) noexcept
            : scheduler(scheduler),
              mailbox(mailbox),
              promise(promise),
              arguments(std::move(arguments))
        {
        }
    };

    auto schedule_recv(const std::shared_ptr<socket_closure> &closure) noexcept -> void
    {
        if (closure->promise.has_result())
        {
            return;
        }
        closure->mailbox
            .send(closure->scheduler, "recv", closure->arguments)
            .then(
                [closure](const Value &value) -> Result
                {
                    if (value.get_null())
                    {
                        closure->scheduler.schedule_delayed(
                            std::chrono::milliseconds(10),
                            [closure]
                            { schedule_recv(closure); });
                    }
                    else
                    {
                        closure->promise.set_result(value);
                    }
                    return {};
                })
            .fail(
                [closure](const Error &error)
                { closure->promise.set_result(error); });
    }

    auto schedule_send(const std::shared_ptr<socket_closure> &socket_closure) noexcept -> void
    {
        if (socket_closure->promise.has_result())
        {
            return;
        }
        const auto send_promise = socket_closure
                                      ->mailbox
                                      .send(socket_closure->scheduler, "send", socket_closure->arguments);
        send_promise.then(
            [socket_closure](const Value &value) -> Result
            {
                if (value.get_null())
                {
                    socket_closure->scheduler.schedule_delayed(
                        std::chrono::milliseconds(10),
                        [socket_closure]
                        { schedule_send(socket_closure); });
                }
                else
                {
                    socket_closure->promise.set_result(value);
                }
                return {};
            });
        send_promise.fail(
            [socket_closure](const Error &error)
            {
                socket_closure->promise.set_result(error);
            });
    }
}

namespace traeger
{
    Socket::Socket(Mailbox &&mailbox) noexcept
        : mailbox_(std::move(mailbox))
    {
    }

    auto Socket::recv(const Scheduler &scheduler) const noexcept -> Promise
    {
        Promise promise{scheduler};
        auto closure = std::make_shared<socket_closure>(
            scheduler,
            mailbox_,
            promise,
            List{});
        schedule_recv(closure);
        return promise;
    }

    auto Socket::send(const Scheduler &scheduler,
                      std::vector<String> &&messages) const noexcept -> Promise
    {
        Promise promise{scheduler};
        List list;
        for (auto &&message : messages)
        {
            list.append(std::move(message));
        }
        auto closure = std::make_shared<socket_closure>(
            scheduler,
            mailbox_, promise,
            make_list(list));
        schedule_send(closure);
        return promise;
    }
}
