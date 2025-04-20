// SPDX-License-Identifier: BSL-1.0

#include <mutex>
#include <condition_variable>
#include <queue>

#include "traeger/value/Value.hpp"
#include "traeger/actor/Queue.hpp"

namespace traeger
{
    struct Queue::impl_type
    {
        impl_type() noexcept
            : closed_(false)
        {
        }

        auto closed() noexcept -> bool
        {
            std::unique_lock lock{mutex_};
            return closed_;
        }

        auto count() noexcept -> std::size_t
        {
            std::unique_lock lock{mutex_};
            return queue_.size();
        }

        auto push(Value &&value) noexcept -> bool
        {
            std::unique_lock lock{mutex_};
            if (!closed_)
            {
                queue_.push(std::move(value));
                condition_.notify_one();
                return true;
            }
            return false;
        }

        auto pop() noexcept -> std::optional<Value>
        {
            std::unique_lock lock{mutex_};
            while (!closed_ &&
                   queue_.empty())
            {
                condition_.wait(lock);
            }
            if (!queue_.empty())
            {
                Value value{std::move(queue_.front())};
                queue_.pop();
                return value;
            }
            return std::nullopt;
        }

        auto pop(List &list) noexcept -> bool
        {
            std::unique_lock lock{mutex_};
            if (queue_.empty())
            {
                return false;
            }
            while (!queue_.empty())
            {
                list.append(std::move(queue_.front()));
                queue_.pop();
            }
            return true;
        }

        auto close() noexcept -> void
        {
            std::unique_lock lock{mutex_};
            closed_ = true;
            condition_.notify_one();
        }

    private:
        bool closed_;
        std::mutex mutex_;
        std::condition_variable condition_;
        std::queue<Value> queue_;
    };

    Queue::Queue() noexcept
        : impl_(std::make_shared<impl_type>())
    {
    }

    auto Queue::closed() const noexcept -> bool
    {
        return impl_->closed();
    }

    auto Queue::count() const noexcept -> std::size_t
    {
        return impl_->count();
    }

    auto Queue::push(const Value &value) const noexcept -> bool
    {
        return impl_->push(Value{value});
    }

    auto Queue::push(Value &&value) const noexcept -> bool
    {
        return impl_->push(std::move(value));
    }

    auto Queue::pop() const noexcept -> std::optional<Value>
    {
        return impl_->pop();
    }

    auto Queue::pop(List &list) const noexcept -> bool
    {
        return impl_->pop(list);
    }

    auto Queue::close() const noexcept -> void
    {
        impl_->close();
    }
}
