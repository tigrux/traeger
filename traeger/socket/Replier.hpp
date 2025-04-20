// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/actor/Promise.hpp>
#include <traeger/socket/Socket.hpp>

namespace traeger
{
    struct Scheduler;

    struct Mailbox;

    struct Replier
    {
        auto reply(const Scheduler &scheduler,
                   const Mailbox &mailbox) const noexcept -> Promise;

    private:
        explicit Replier(Socket &&socket) noexcept;
        friend struct Context;

        Socket router_;
    };
}

struct traeger_replier_t : traeger::Replier
{
};
