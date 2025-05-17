// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/value/Value.hpp>
#include <traeger/actor/Mailbox.hpp>

#include "module.h"

namespace traeger
{
    struct Scheduler;

    struct Module
    {
        ~Module() noexcept;

        Module() = delete;

        auto mailbox() const noexcept -> Mailbox;

        static auto from_path(const String &path,
                              const Map &configuration) noexcept -> std::pair<std::optional<Module>, String>;

    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl_;

        explicit Module(std::shared_ptr<impl_type> &&impl) noexcept;
    };
}

struct traeger_module_t final : traeger::Module
{
};
