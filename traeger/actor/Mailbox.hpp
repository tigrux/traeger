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
            virtual auto
            send(const Scheduler &scheduler,
                 const String &name,
                 const List &arguments) noexcept -> Promise = 0;
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

struct traeger_mailbox_t : traeger::Mailbox
{
};
