// SPDX-License-Identifier: BSL-1.0

#include "traeger/value/Value.hpp"
#include "traeger/value/Value_impl.hpp"
#include "traeger/value/Variant.hpp"

namespace
{
    template <class... Types>
    struct overload : Types...
    {
        using Types::operator()...;
    };

    template <class... Types>
    overload(Types...) -> overload<Types...>;
}

namespace traeger
{
    auto value_from_variant(const Variant &variant) noexcept -> Value
    {
        return std::visit(
            [](const auto &variant)
            { return Value{variant}; },
            variant);
    }

    auto value_from_variant(Variant &&variant) noexcept -> Value
    {
        return std::visit(
            [](auto &&variant)
            { return Value{std::move(variant)}; },
            std::move(variant));
    }

    auto value_to_variant(const Value &value) noexcept -> Variant
    {
        return std::visit(
            overload{
                [](auto variant)
                { return Variant{variant}; },
                [](const Value::impl_type::list_type &values)
                { return Variant{List{values}}; },
                [](const Value::impl_type::map_type &values)
                { return Variant{Map{values}}; }},
            value.impl().variant);
    }

    auto value_to_variant(Value &&value) noexcept -> Variant
    {
        return std::visit(
            overload{
                [](auto variant)
                { return Variant{variant}; },
                [](Value::impl_type::list_type &&values)
                { return Variant{List{std::move(values)}}; },
                [](Value::impl_type::map_type &&values)
                { return Variant{Map{std::move(values)}}; }},
            std::move(value.impl().variant));
    }
}
