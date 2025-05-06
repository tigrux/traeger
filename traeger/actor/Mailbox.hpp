// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <memory>

#include <traeger/actor/Promise.hpp>

namespace traeger
{
    struct Scheduler;

    struct Mailbox
    {
        struct Interface
        {
            virtual Promise
            send(const Scheduler &scheduler,
                 const String &name,
                 const List &arguments) noexcept = 0;
        };

        Mailbox() = delete;

        Mailbox(const std::shared_ptr<Interface> &interface) noexcept;

        auto send(const Scheduler &scheduler,
                  const String &name,
                  const List &arguments) const noexcept -> Promise;

    private:
        std::shared_ptr<Interface> interface_;
    };
}

struct traeger_mailbox_t final : traeger::Mailbox
{
};
