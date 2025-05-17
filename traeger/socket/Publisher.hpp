// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/format/Format.hpp>
#include <traeger/actor/Promise.hpp>
#include <traeger/socket/Socket.hpp>

namespace traeger
{
    struct Scheduler;

    struct Publisher
    {
        auto publish(const Scheduler &scheduler,
                     const String &topic,
                     const Value &value) const noexcept -> Promise;

    private:
        Publisher(Socket &&socket,
                  Format format) noexcept;
        friend struct Context;

        Socket publisher_;
        Format format_;
    };
}

struct traeger_publisher_t final : traeger::Publisher
{
};
