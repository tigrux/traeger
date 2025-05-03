// SPDX-License-Identifier: BSL-1.0

#include <mutex>
#include <functional>
#include <utility>
#include <queue>
#include <memory>

#include "traeger/actor/Result.hpp"
#include "traeger/actor/Promise.hpp"

namespace traeger
{
    struct Promise::impl_type
    {
        impl_type(const Scheduler &scheduler) noexcept
            : scheduler_(scheduler)
        {
        }

        auto set(Result &&result) noexcept -> bool
        {
            std::lock_guard lock{mutex_};
            if (result_.type() == Result::Type::Undefined)
            {
                result_ = std::move(result);
                schedule_callbacks();
                return true;
            }
            return false;
        }

        auto then(ResultCallback &&result_callback) noexcept -> Promise
        {
            std::lock_guard lock{mutex_};
            Promise promise{scheduler_};
            value_callbacks_.push(
                [promise, result_callback = std::move(result_callback)](const Value &value)
                { promise.set_result(result_callback(value)); });
            error_callbacks_.push(
                [promise](const Error &error)
                { promise.set_result(error); });
            schedule_callbacks();
            return promise;
        }

        auto then(PromiseCallback &&promise_callback) noexcept -> Promise
        {
            std::lock_guard lock{mutex_};
            Promise promise{scheduler_};
            value_callbacks_.push(
                [promise, promise_callback = std::move(promise_callback)](const Value &value)
                { promise.set_result_from_promise(promise_callback(value)); });
            error_callbacks_.push([promise](const Error &error)
                                  { promise.set_result(error); });
            schedule_callbacks();
            return promise;
        }

        auto fail(ErrorCallback &&error_callback) noexcept -> void
        {
            std::lock_guard lock{mutex_};
            error_callbacks_.push(std::move(error_callback));
            schedule_callbacks();
        }

        auto scheduler() const noexcept -> const Scheduler &
        {
            return scheduler_;
        }

        auto result() noexcept -> Result
        {
            std::lock_guard lock{mutex_};
            return result_;
        }

        auto has_result() noexcept -> bool
        {
            std::lock_guard lock{mutex_};
            return result_.type() != Result::Type::Undefined;
        }

        auto schedule_callbacks() noexcept -> void
        {
            switch (result_.type())
            {
            case Result::Type::Value:
            {
                const Value value{*result_.value()};
                while (!value_callbacks_.empty())
                {
                    scheduler_.schedule(
                        [value_callback = std::move(value_callbacks_.front()), value]
                        { value_callback(value); });
                    value_callbacks_.pop();
                }
                if (!error_callbacks_.empty())
                {
                    ErrorCallbacks empty;
                    error_callbacks_.swap(empty);
                }
                break;
            }
            case Result::Type::Error:
            {
                const Error error{*result_.error()};
                while (!error_callbacks_.empty())
                {
                    scheduler_.schedule(
                        [error_callback = std::move(error_callbacks_.front()), error]
                        { error_callback(error); });
                    error_callbacks_.pop();
                }
                if (!value_callbacks_.empty())
                {
                    ValueCallbacks empty;
                    value_callbacks_.swap(empty);
                }
                break;
            }
            case Result::Type::Undefined:
                break;
            }
        }

    private:
        Scheduler scheduler_;
        Result result_;
        std::mutex mutex_;
        ValueCallbacks value_callbacks_;
        ErrorCallbacks error_callbacks_;
    };

    Promise::Promise(const Scheduler &scheduler) noexcept
        : impl_(std::make_shared<impl_type>(scheduler))
    {
    }

    Promise::Promise(const Promise &other) noexcept
        : impl_(other.impl_)
    {
    }

    Promise::Promise(Promise &&other) noexcept
        : impl_(std::move(other.impl_))
    {
    }

    Promise &Promise::operator=(const Promise &other) noexcept
    {
        impl_ = other.impl_;
        return *this;
    }

    Promise &Promise::operator=(Promise &&other) noexcept
    {
        impl_ = std::move(other.impl_);
        return *this;
    }

    auto Promise::scheduler() const noexcept -> const Scheduler &
    {
        return impl_->scheduler();
    }

    auto Promise::result() const noexcept -> Result
    {
        return impl_->result();
    }

    auto Promise::has_result() const noexcept -> bool
    {
        return impl_->has_result();
    }

    auto Promise::set_result(const Result &result) const noexcept -> bool
    {
        return impl_->set(Result{result});
    }

    auto Promise::set_result(Result &&result) const noexcept -> bool
    {
        return impl_->set(std::move(result));
    }

    auto Promise::set_result_from_promise(const Promise &promise) const noexcept -> void
    {
        promise.then(
            [impl = impl_](const Value &value) -> Result
            {
                impl->set(Result{value});
                return Result{};
            });
        promise.fail(
            [impl = impl_](const Error &error) -> void
            {
                impl->set(Result{error});
            });
    }

    auto Promise::then(ResultCallback &&result_callback) const noexcept -> Promise
    {
        return impl_->then(std::move(result_callback));
    }

    auto Promise::then(PromiseCallback &&promise_callback) const noexcept -> Promise
    {
        return impl_->then(std::move(promise_callback));
    }

    auto Promise::fail(ErrorCallback &&error_callback) const noexcept -> void
    {
        return impl_->fail(std::move(error_callback));
    }
}
