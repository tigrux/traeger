// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <optional>
#include <memory>

#include <traeger/value/Value.hpp>

namespace traeger
{
    struct Queue
    {
        Queue() noexcept;

        auto closed() const noexcept -> bool;

        auto count() const noexcept -> std::size_t;

        auto push(const Value &value) const noexcept -> bool;

        auto push(Value &&value) const noexcept -> bool;

        auto pop() const noexcept -> std::optional<Value>;

        auto pop(List &list) const noexcept -> bool;

        auto close() const noexcept -> void;

    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl_;
    };
}

struct traeger_queue_t final : traeger::Queue
{
};
