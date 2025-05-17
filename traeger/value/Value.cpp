// SPDX-License-Identifier: BSL-1.0

#include <variant>
#include <iomanip>
#include <ostream>
#include <utility>
#include <ios>
#include <optional>

#include "traeger/value/Value.hpp"
#include "traeger/value/Value_impl.hpp"

namespace
{
    using namespace traeger;

    template <class... Types>
    struct overload : Types...
    {
        using Types::operator()...;
    };

    template <class... Types>
    overload(Types...) -> overload<Types...>;

    auto string_to_bool(const String &str) -> std::optional<Bool>
    {
        if (str == "true")
        {
            return {true};
        }
        if (str == "false")
        {
            return {false};
        }
        return std::nullopt;
    }

    auto string_to_int(const String &str) -> std::optional<Int>
    {
        std::size_t pos = 0;
        const auto result = std::stol(str, &pos);
        if (pos == str.size())
        {
            return {result};
        }
        return std::nullopt;
    }

    auto string_to_uint(const String &str) -> std::optional<UInt>
    {
        try
        {
            std::size_t pos = 0;
            const auto result = std::stoul(str, &pos);
            if (pos == str.size())
            {
                return {result};
            }
        }
        catch (...)
        {
        }
        return std::nullopt;
    }

    auto string_to_float(const String &str) -> std::optional<Float>
    {
        try
        {
            std::size_t pos = 0;
            const auto result = std::stod(str, &pos);
            if (pos == str.size())
            {
                return {result};
            }
        }
        catch (...)
        {
        }
        return std::nullopt;
    }
}

namespace traeger
{
    Value::impl_type::impl_type() noexcept
        : variant()
    {
    }

    Value::impl_type::impl_type(const variant_type &variant) noexcept
        : variant(variant)
    {
    }

    Value::impl_type::impl_type(variant_type &&variant) noexcept
        : variant(std::move(variant))
    {
    }

    auto Value::impl_type::get_list() const noexcept -> const list_type *
    {
        return std::get_if<list_type>(&variant);
    }

    auto Value::impl_type::get_map() const noexcept -> const map_type *
    {
        return std::get_if<map_type>(&variant);
    }

    auto Value::impl_type::get_string() const noexcept -> const string_type *
    {
        return std::get_if<string_type>(&variant);
    }

    Value::~Value() noexcept
    {
        impl().~impl_type();
    }

    Value::Value() noexcept
    {
        new (impl_) impl_type{};
    }

    Value::Value(const Value &other) noexcept
        : Value()
    {
        impl().variant = other.impl().variant;
    }

    Value::Value(Value &&other) noexcept
        : Value()
    {
        impl().variant = std::move(other.impl().variant);
        other.impl().variant.emplace<Null>();
    }

    Value::Value(Null) noexcept
        : Value()
    {
        impl().variant.emplace<Null>();
    }

    Value::Value(Bool variant) noexcept
        : Value()
    {
        impl().variant.emplace<Bool>(variant);
    }

    Value::Value(Int variant) noexcept
        : Value()
    {
        impl().variant.emplace<Int>(variant);
    }

    Value::Value(const std::int32_t variant) noexcept
        : Value()
    {
        impl().variant.emplace<Int>(static_cast<Int>(variant));
    }

    Value::Value(const std::uint32_t variant) noexcept
        : Value()
    {
        impl().variant.emplace<Int>(static_cast<Int>(variant));
    }

    Value::Value(UInt variant) noexcept
        : Value()
    {
        if (static_cast<Int>(variant) >= 0)
        {
            impl().variant.emplace<Int>(static_cast<Int>(variant));
        }
        else
        {
            impl().variant.emplace<UInt>(variant);
        }
    }

    Value::Value(Float variant) noexcept
        : Value()
    {
        impl().variant.emplace<Float>(variant);
    }

    Value::Value(const String &variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::string_type>(variant);
    }

    Value::Value(String &&variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::string_type>(std::move(variant));
    }

    Value::Value(const char *variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::string_type>(variant);
    }

    Value::Value(const List &variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::list_type>(variant.impl().persistent());
    }

    Value::Value(List &&variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::list_type>(std::move(variant.impl()).persistent());
    }

    Value::Value(const Map &variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::map_type>(variant.impl().persistent());
    }

    Value::Value(Map &&variant) noexcept
        : Value()
    {
        impl().variant.emplace<impl_type::map_type>(std::move(variant.impl()).persistent());
    }

    auto Value::operator=(const Value &other) noexcept -> Value &
    {
        if (this != &other)
        {
            impl().variant = other.impl().variant;
        }
        return *this;
    }

    auto Value::operator=(Value &&other) noexcept -> Value &
    {
        impl().variant = std::move(other.impl().variant);
        other.impl().variant.emplace<Null>();
        return *this;
    }

    auto Value::operator==(const Value &other) const noexcept -> bool
    {
        return impl().variant == other.impl().variant;
    }

    auto Value::operator!=(const Value &other) const noexcept -> bool
    {
        return impl().variant != other.impl().variant;
    }

    auto Value::type() const noexcept -> Type
    {
        return static_cast<Type>(impl().variant.index());
    }

    auto Value::type_name() const noexcept -> const String &
    {
        return type_name(type());
    }

    auto Value::get_null() const noexcept -> std::optional<Null>
    {
        if (type() == Type::Null)
        {
            return {nullptr};
        }
        return std::nullopt;
    }

    auto Value::get_bool() const noexcept -> std::optional<Bool>
    {
        switch (type())
        {
        case Type::Bool:
            return std::get<Bool>(impl().variant);
        case Type::String:
            return string_to_bool(impl().get_string()->get());
        default:
            break;
        }
        return std::nullopt;
    }

    auto Value::get_int() const noexcept -> std::optional<Int>
    {
        switch (type())
        {
        case Type::Int:
            return std::get<Int>(impl().variant);
        case Type::String:
            return string_to_int(impl().get_string()->get());
        default:
            break;
        }
        return std::nullopt;
    }

    auto Value::get_uint() const noexcept -> std::optional<UInt>
    {
        switch (type())
        {
        case Type::Int:
        {
            const auto value = std::get<Int>(impl().variant);
            if (value >= 0)
            {
                return {static_cast<UInt>(value)};
            }
            break;
        }
        case Type::UInt:
            return std::get<UInt>(impl().variant);
        case Type::String:
            return string_to_uint(impl().get_string()->get());
        default:
            break;
        }
        return std::nullopt;
    }

    auto Value::get_float() const noexcept -> std::optional<Float>
    {
        switch (type())
        {
        case Type::Int:
            return static_cast<Float>(std::get<Int>(impl().variant));
        case Type::UInt:
            return static_cast<Float>(std::get<UInt>(impl().variant));
        case Type::Float:
            return std::get<Float>(impl().variant);
        case Type::String:
            return string_to_float(impl().get_string()->get());
        default:
            break;
        }
        return std::nullopt;
    }

    auto Value::get_string() const noexcept -> const String *
    {
        if (type() == Type::String)
        {
            return &impl().get_string()->get();
        }
        return nullptr;
    }

    auto Value::get_list() const noexcept -> std::optional<List>
    {
        if (type() == Type::List)
        {
            return List{List::impl_type{*impl().get_list()}};
        }
        return std::nullopt;
    }

    auto Value::get_map() const noexcept -> std::optional<Map>
    {
        if (type() == Type::Map)
        {
            return Map{Map::impl_type{*impl().get_map()}};
        }
        return std::nullopt;
    }

    auto operator<<(std::ostream &os,
                    const Value &value) noexcept -> std::ostream &
    {
        std::visit(
            overload{
                [&os](Null)
                { os << "null"; },
                [&os](const Bool variant)
                { os << std::boolalpha << variant << std::noboolalpha; },
                [&os](const Float variant)
                { os << std::showpoint << variant << std::noshowpoint; },
                [&os](const Value::impl_type::string_type &boxed)
                { os << std::quoted(boxed.get()); },
                [&os](const Value::impl_type::list_type &values)
                { os << List{List::impl_type{values}}; },
                [&os](const Value::impl_type::map_type &values)
                { os << Map{Map::impl_type{values}}; },
                [&os](auto variant)
                { os << variant; }},
            value.impl().variant);
        return os;
    }

    auto Value::type_name(Type type) noexcept -> const String &
    {
        static const String types[]{
            "Null",
            "Bool",
            "Int",
            "UInt",
            "Float",
            "String",
            "List",
            "Map",
        };
        return types[static_cast<int>(type)];
    }
}
