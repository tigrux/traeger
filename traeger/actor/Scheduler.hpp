// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>
#include <memory>
#include <chrono>

#include <traeger/value/Types.hpp>

namespace traeger
{
    auto to_microseconds(Float seconds) noexcept -> std::chrono::microseconds;

    using Work = std::function<void()>;

    using Clock = std::chrono::steady_clock;

    using Duration = std::chrono::duration<Clock::rep, Clock::period>;

    struct Threads
    {
        unsigned int count;
    };

    struct Scheduler
    {
    public:
        Scheduler() = delete;

        Scheduler(Threads threads) noexcept;

        auto schedule(Work &&work) const noexcept -> void;

        auto schedule_delayed(const Duration &delay,
                              Work &&work) const noexcept -> void;

        auto count() const noexcept -> std::size_t;

    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl_;
    };
}

struct traeger_work_t final : traeger::Work
{
};

struct traeger_scheduler_t final : traeger::Scheduler
{
};
