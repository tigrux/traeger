// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <memory>
#include <optional>

#include <traeger/actor/Mailbox.hpp>

namespace traeger
{
    struct Group
    {
        Group() noexcept;

        auto add(const String &name,
                 const Mailbox &mailbox) const noexcept -> void;

        auto mailbox() const noexcept -> Mailbox;

        auto find(const String &member) const noexcept -> std::optional<Mailbox>;

    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl_;
    };
}