// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>
#include <memory>

#include <traeger/actor/Result.hpp>
#include <traeger/actor/Mailbox.hpp>

namespace traeger
{
    using Function = std::function<Result(List)>;

    struct StatelessActor
    {
        ~StatelessActor() noexcept;

        StatelessActor() noexcept;

        StatelessActor(const StatelessActor &other) = delete;

        StatelessActor(StatelessActor &&other) noexcept;

        auto define_reader(const String &name,
                           const Function &function) const noexcept -> void;

        auto define_writer(const String &name,
                           const Function &function) const noexcept -> void;

        auto mailbox() const noexcept -> Mailbox;

    private:
        struct impl_type;
        std::unique_ptr<impl_type> impl_;
    };
}

struct traeger_function_t final : traeger::Function
{
};

struct traeger_actor_t final : traeger::StatelessActor
{
};
