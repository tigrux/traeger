// SPDX-License-Identifier: BSL-1.0

#include "traeger/actor/Promise.hpp"
#include "traeger/actor/Mailbox.hpp"

namespace traeger
{
    Mailbox::Mailbox(const std::shared_ptr<Interface> &interface) noexcept
        : interface_(interface)
    {
    }

    auto Mailbox::send(const Scheduler &scheduler,
                       const String &name,
                       const List &arguments) const noexcept -> Promise
    {
        return interface_->send(scheduler, name, arguments);
    }
}
