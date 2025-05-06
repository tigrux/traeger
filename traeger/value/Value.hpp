// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

#include <traeger/value/Types.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Map.hpp>

namespace traeger
{
    template <typename Arg>
    auto constexpr assert_is_variant_type() -> void
    {
        static_assert(std::disjunction<std::is_same<Arg, Null>,
                                       std::is_same<Arg, Bool>,
                                       std::is_same<Arg, Int>,
                                       std::is_same<Arg, UInt>,
                                       std::is_same<Arg, Float>,
                                       std::is_same<Arg, String>,
                                       std::is_same<Arg, List>,
                                       std::is_same<Arg, Map>>::value);
    }

    struct Value
    {
        struct impl_type;

        inline auto impl() const & noexcept -> const impl_type &
        {
            return *reinterpret_cast<const impl_type *>(&impl_);
        }

        inline auto impl() & noexcept -> impl_type &
        {
            return *reinterpret_cast<impl_type *>(&impl_);
        }

        enum class Type : int
        {
            Null = TRAEGER_VALUE_TYPE_NULL,
            Bool = TRAEGER_VALUE_TYPE_BOOL,
            Int = TRAEGER_VALUE_TYPE_INT,
            UInt = TRAEGER_VALUE_TYPE_UINT,
            Float = TRAEGER_VALUE_TYPE_FLOAT,
            String = TRAEGER_VALUE_TYPE_STRING,
            List = TRAEGER_VALUE_TYPE_LIST,
            Map = TRAEGER_VALUE_TYPE_MAP,
        };

        ~Value() noexcept;

        Value() noexcept;

        Value(const Value &other) noexcept;

        Value(Value &&other) noexcept;

        Value(Null variant) noexcept;

        Value(Bool variant) noexcept;

        Value(Int variant) noexcept;

        Value(std::int32_t variant) noexcept;

        Value(std::uint32_t variant) noexcept;

        Value(UInt variant) noexcept;

        Value(Float variant) noexcept;

        Value(const String &variant) noexcept;

        Value(String &&variant) noexcept;

        Value(const char *variant) noexcept;

        Value(const List &variant) noexcept;

        Value(List &&variant) noexcept;

        Value(const Map &variant) noexcept;

        Value(Map &&variant) noexcept;

        auto operator=(const Value &other) noexcept -> Value &;

        auto operator=(Value &&other) noexcept -> Value &;

        auto operator==(const Value &other) const noexcept -> bool;

        auto operator!=(const Value &other) const noexcept -> bool;

        auto type() const noexcept -> Type;

        template <typename Arg>
        static constexpr auto type() noexcept -> Type
        {
            assert_is_variant_type<Arg>();
            if constexpr (std::is_same<Arg, Null>::value)
            {
                return Type::Null;
            }
            if constexpr (std::is_same<Arg, Bool>::value)
            {
                return Type::Bool;
            }
            if constexpr (std::is_same<Arg, Int>::value)
            {
                return Type::Int;
            }
            if constexpr (std::is_same<Arg, UInt>::value)
            {
                return Type::UInt;
            }
            if constexpr (std::is_same<Arg, Float>::value)
            {
                return Type::Float;
            }
            if constexpr (std::is_same<Arg, String>::value)
            {
                return Type::String;
            }
            if constexpr (std::is_same<Arg, List>::value)
            {
                return Type::List;
            }
            if constexpr (std::is_same<Arg, Map>::value)
            {
                return Type::Map;
            }
        }

        auto type_name() const noexcept -> const String &;

        static auto type_name(Type type) noexcept -> const String &;

        template <typename Arg>
        constexpr auto get() const noexcept -> std::optional<Arg>
        {
            assert_is_variant_type<Arg>();
            if constexpr (std::is_same<Arg, Null>::value)
            {
                return get_null();
            }
            if constexpr (std::is_same<Arg, Bool>::value)
            {
                return get_bool();
            }
            if constexpr (std::is_same<Arg, Int>::value)
            {
                return get_int();
            }
            if constexpr (std::is_same<Arg, UInt>::value)
            {
                return get_uint();
            }
            if constexpr (std::is_same<Arg, Float>::value)
            {
                return get_float();
            }
            if constexpr (std::is_same<Arg, String>::value)
            {
                if (const auto *ptr_string = get_string(); ptr_string)
                {
                    return std::optional<String>{*ptr_string};
                }
                else
                {
                    return std::nullopt;
                }
            }
            if constexpr (std::is_same<Arg, List>::value)
            {
                return get_list();
            }
            if constexpr (std::is_same<Arg, Map>::value)
            {
                return get_map();
            }
        }

        auto get_null() const noexcept -> std::optional<Null>;

        auto get_bool() const noexcept -> std::optional<Bool>;

        auto get_int() const noexcept -> std::optional<Int>;

        auto get_uint() const noexcept -> std::optional<UInt>;

        auto get_float() const noexcept -> std::optional<Float>;

        auto get_string() const noexcept -> const String *;

        auto get_list() const noexcept -> std::optional<List>;

        auto get_map() const noexcept -> std::optional<Map>;

        using layout_type = std::variant<
            Null,
            Bool,
            Int,
            UInt,
            Float,
            std::uintptr_t,
            List::layout_type,
            Map::layout_type>;

    private:
        std::byte impl_[sizeof(layout_type)];
    };

    auto operator<<(std::ostream &os,
                    const Value &value) noexcept -> std::ostream &;

    template <int Index, typename Arg>
    auto List::get_index_arg() const -> Arg
    {
        const auto *ptr_value = find(Index);
        if (!ptr_value)
        {
            throw std::runtime_error(
                "invalid index in argument " + std::to_string(Index));
        }

        if constexpr (std::is_same<Arg, Value>::value)
        {
            return *ptr_value;
        }
        else
        {
            const auto optional = ptr_value->get<Arg>();
            if (!optional)
            {
                throw std::runtime_error(
                    "invalid cast in argument " + std::to_string(Index) +
                    " from type " + ptr_value->type_name() +
                    " to " + Value::type_name(Value::type<Arg>()));
            }
            return optional.value();
        }
    }

    template <typename Arg, typename... Args>
    auto Map::get(const String &key, Arg &arg) const noexcept -> std::pair<bool, String>
    {
        const auto *ptr_value = find(key);
        if (!ptr_value)
        {
            return {false, "invalid key " + key};
        }
        const auto optional = ptr_value->get<Arg>();
        if (!optional)
        {
            return {false, "invalid cast from type " +
                               ptr_value->type_name() +
                               " to " +
                               Value::type_name(Value::type<Arg>())};
        }
        arg = optional.value();
        return {true, String{}};
    }

    template <typename Variant, typename... Args>
    auto Map::get(const String &key, Variant &variant, Args &&...args) const noexcept -> std::pair<bool, String>
    {
        const auto ok_error = get(key, variant);
        if (!ok_error.first)
        {
            return ok_error;
        }
        if constexpr (sizeof...(args) > 0)
        {
            return get(std::forward<Args>(args)...);
        }
        else
        {
            return ok_error;
        }
    }
}

struct traeger_value_t final : traeger::Value
{
};
