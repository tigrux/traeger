// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <vector>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>
#include <utility>
#include <atomic>
#include <mutex>
#include <cmath>

#include "traeger/actor/Scheduler.hpp"

namespace traeger
{
    auto to_microseconds(const Float seconds) noexcept -> std::chrono::microseconds
    {
        const std::intmax_t microseconds = std::lround(seconds * std::micro::den);
        return std::chrono::microseconds(microseconds);
    }

    struct Scheduler::impl_type
    {
        struct ScheduledWork
        {
            std::chrono::time_point<Clock> time_point;
            Work work;

            auto operator<(const ScheduledWork &other) const noexcept -> bool
            {
                return time_point > other.time_point;
            }
        };

        ~impl_type() noexcept
        {
            stop();
            for (auto &thread : threads_)
            {
                thread.join();
            }
        }

        explicit impl_type(const unsigned int threads_count) noexcept
            : active_(true),
              threads_count_(threads_count),
              active_threads_count_(0)
        {
            start();
        }

        auto schedule(const Duration &delay,
                      Work &&work) noexcept -> void
        {
            std::unique_lock lock{mutex_};
            if (active_)
            {
                if (delay != Duration::zero())
                {
                    scheduled_queue_.push(ScheduledWork{Clock::now() + delay, std::move(work)});
                }
                else
                {
                    queue_.push(std::move(work));
                }
                condition_.notify_one();
            }
        }

        auto count() noexcept -> std::size_t
        {
            std::unique_lock lock{mutex_};
            return queue_.size() +
                   scheduled_queue_.size() +
                   active_threads_count_;
        }

        auto start() noexcept -> void
        {
            threads_.reserve(threads_count_);
            for (unsigned int i = 0; i < threads_count_; ++i)
            {
                threads_.emplace_back(&impl_type::worker, this);
            }
        }

        auto stop() noexcept -> void
        {
            std::unique_lock lock{mutex_};
            active_ = false;
            condition_.notify_all();
            condition_.wait(lock, [this]
                            { return threads_count_ == 0; });
        }

        auto worker() noexcept -> void
        {
            while (true)
            {
                Work work;
                if (!next(work))
                {
                    break;
                }
                ++active_threads_count_;
                work();
                --active_threads_count_;
            }
        }

        auto next(Work &work) noexcept -> bool
        {
            std::unique_lock lock{mutex_};
            for (;;)
            {
                if (active_ &&
                    !scheduled_queue_.empty() &&
                    queue_.empty())
                {
                    const auto &[time_point, work] = scheduled_queue_.top();
                    condition_.wait_until(lock, time_point);
                }
                else
                {
                    while (active_ &&
                           scheduled_queue_.empty() &&
                           queue_.empty())
                    {
                        condition_.wait(lock);
                    }
                }

                if (active_)
                {
                    if (!scheduled_queue_.empty())
                    {
                        if (const auto &[top_time_point, top_work] = scheduled_queue_.top();
                            Clock::now() >= top_time_point)
                        {
                            work = top_work;
                            scheduled_queue_.pop();
                            return true;
                        }
                    }

                    if (!queue_.empty())
                    {
                        work = std::move(queue_.front());
                        queue_.pop();
                        return true;
                    }
                }
                else if (queue_.empty())
                {
                    --threads_count_;
                    condition_.notify_one();
                    return false;
                }
            }
        }

    private:
        bool active_;
        unsigned int threads_count_;
        std::atomic<unsigned int> active_threads_count_;
        std::vector<std::thread> threads_;

        std::mutex mutex_;
        std::condition_variable condition_;
        std::queue<Work> queue_;
        std::priority_queue<ScheduledWork> scheduled_queue_;
    };

    Scheduler::Scheduler(Threads threads) noexcept
        : impl_(std::make_shared<impl_type>(threads.count))
    {
    }

    auto Scheduler::schedule(Work &&work) const noexcept -> void
    {
        impl_->schedule(Duration::zero(), std::move(work));
    }

    auto Scheduler::schedule_delayed(const Duration &delay,
                                     Work &&work) const noexcept -> void
    {
        impl_->schedule(delay, std::move(work));
    }

    auto Scheduler::count() const noexcept -> std::size_t
    {
        return impl_.use_count() + impl_->count() - 1;
    }
}
