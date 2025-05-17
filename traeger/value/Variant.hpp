// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <variant>

#include <traeger/value/Types.hpp>

namespace traeger
{
    using Variant = std::variant<
        Null,
        Bool,
        Int,
        UInt,
        Float,
        String,
        List,
        Map>;

    auto value_from_variant(const Variant &variant) noexcept -> Value;

    auto value_from_variant(Variant &&variant) noexcept -> Value;

    auto value_to_variant(const Value &value) noexcept -> Variant;

    auto value_to_variant(Value &&value) noexcept -> Variant;
}
