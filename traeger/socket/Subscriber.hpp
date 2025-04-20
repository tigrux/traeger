// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <functional>

#include <traeger/actor/Promise.hpp>
#include <traeger/socket/Socket.hpp>

namespace traeger
{
    struct Scheduler;

    struct Promise;

    struct Subscriber
    {
        using Callback = std::function<void(String, Value)>;

        auto listen(const Scheduler &scheduler,
                    Callback &&callback) const noexcept -> Promise;

    private:
        explicit Subscriber(Socket &&socket) noexcept;
        friend struct Context;

        Socket subscriber_;
    };
}

struct traeger_subscriber_t : traeger::Subscriber
{
};
