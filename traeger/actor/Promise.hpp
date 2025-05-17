// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>
#include <memory>
#include <queue>

#include <traeger/actor/Result.hpp>
#include <traeger/actor/Scheduler.hpp>

namespace traeger
{
    struct Error;

    struct Promise
    {
        using ResultCallback = std::function<Result(const Value &)>;

        using PromiseCallback = std::function<Promise(const Value &)>;

        using ValueCallback = std::function<void(const Value &)>;

        using ErrorCallback = std::function<void(const Error &)>;

        using ValueCallbacks = std::queue<ValueCallback>;

        using ErrorCallbacks = std::queue<ErrorCallback>;

        Promise(const Promise &other) noexcept;

        Promise(Promise &&other) noexcept;

        explicit Promise(const Scheduler &scheduler) noexcept;

        Promise &operator=(const Promise &other) noexcept;

        Promise &operator=(Promise &&other) noexcept;

        auto scheduler() const noexcept -> const Scheduler &;

        auto result() const noexcept -> Result;

        auto has_result() const noexcept -> bool;

        auto set_result(const Result &result) const noexcept -> bool;

        auto set_result(Result &&result) const noexcept -> bool;

        auto set_result_from_promise(const Promise &promise) const noexcept -> void;

        auto then(ResultCallback &&result_callback) const noexcept -> Promise;

        auto then(PromiseCallback &&promise_callback) const noexcept -> Promise;

        auto fail(ErrorCallback &&error_callback) const noexcept -> void;

    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl_;
    };
}

struct traeger_promise_t final : traeger::Promise
{
};
