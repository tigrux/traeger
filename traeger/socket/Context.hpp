// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include <traeger/value/Types.hpp>

namespace traeger
{
    struct Format;

    struct Replier;

    struct Requester;

    struct Publisher;

    struct Subscriber;

    struct Context
    {
        Context() noexcept;

#if defined(__ZMQ_HPP_INCLUDED__)
        explicit operator zmq::context_t &() const;
#endif

        auto replier(const String &address) const noexcept -> std::pair<std::optional<Replier>, String>;

        auto requester(const String &address, const Format &format) const noexcept -> std::pair<std::optional<Requester>, String>;

        auto publisher(const String &address, const Format &format) const noexcept -> std::pair<std::optional<Publisher>, String>;

        auto subscriber(const String &address, const std::vector<String> &topics) const noexcept -> std::pair<std::optional<Subscriber>, String>;

    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl_;
    };
}

struct traeger_context_t final : traeger::Context
{
};
