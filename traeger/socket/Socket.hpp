// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <vector>

#include <traeger/actor/Promise.hpp>
#include <traeger/actor/Mailbox.hpp>

namespace traeger
{
    struct Scheduler;

    struct Socket
    {
        auto send(const Scheduler &scheduler,
                  std::vector<String> &&messages) const noexcept -> Promise;

        auto recv(const Scheduler &scheduler) const noexcept -> Promise;

    private:
        explicit Socket(Mailbox &&mailbox) noexcept;
        friend struct Context;

        Mailbox mailbox_;
    };
}
