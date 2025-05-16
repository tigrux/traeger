// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <memory>

#include <traeger/actor/Promise.hpp>

struct traeger_mailbox_interface_t
{
    virtual ~traeger_mailbox_interface_t() = default;

    virtual traeger::Promise
    send(const traeger::Scheduler &scheduler,
         const traeger::String &name,
         const traeger::List &arguments) noexcept = 0;
};

namespace traeger
{
    struct Scheduler;

    struct Mailbox
    {
        using Interface = traeger_mailbox_interface_t;

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
