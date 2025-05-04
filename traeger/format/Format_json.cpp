// SPDX-License-Identifier: BSL-1.0

#include <variant>

#include <nlohmann/json.hpp>

#include "traeger/value/Value_impl.hpp"
#include "traeger/format/Format.hpp"

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

    auto list_from_json(const nlohmann::json &object) -> List;
    auto map_from_json(const nlohmann::json &object) -> Map;
    auto value_from_json(const nlohmann::json &object) -> Value;

    auto json_from(const Value::impl_type::list_type &values) -> nlohmann::json;
    auto json_from(const Value::impl_type::map_type &values) -> nlohmann::json;
    auto json_from(const Value &value) -> nlohmann::json;

    auto list_from_json(const nlohmann::json &object) -> List
    {
        List list;
        const auto end = object.end();
        const auto begin = object.begin();
        for (auto it = begin; it != end; ++it)
        {
            list.append(value_from_json(it.value()));
        }
        return list;
    }

    auto map_from_json(const nlohmann::json &object) -> Map
    {
        Map map;
        const auto begin = object.begin();
        const auto end = object.end();
        for (auto it = begin; it != end; ++it)
        {
            map.set(it.key(), value_from_json(it.value()));
        }
        return map;
    }

    auto value_from_json(const nlohmann::json &object) -> Value
    {
        switch (object.type())
        {
        default:
        case nlohmann::json::value_t::null:
            return nullptr;
        case nlohmann::json::value_t::boolean:
            return static_cast<Bool>(object);
        case nlohmann::json::value_t::number_integer:
            return static_cast<Int>(object);
        case nlohmann::json::value_t::number_unsigned:
            return static_cast<UInt>(object);
        case nlohmann::json::value_t::number_float:
            return static_cast<Float>(object);
        case nlohmann::json::value_t::string:
            return static_cast<String>(object);
        case nlohmann::json::value_t::array:
            return list_from_json(object);
        case nlohmann::json::value_t::object:
            return map_from_json(object);
        }
    }

    auto json_from(const Value::impl_type::list_type &values) -> nlohmann::json
    {
        nlohmann::json object = nlohmann::json::array();
        for (const auto &value : values)
        {
            object.emplace_back(json_from(value));
        }
        return object;
    }

    auto json_from(const Value::impl_type::map_type &values) -> nlohmann::json
    {
        nlohmann::json object;
        for (const auto &[key, value] : values)
        {
            object.emplace(key, json_from(value));
        }
        return object;
    }

    auto json_from(const Value &value) -> nlohmann::json
    {
        return std::visit(
            overload{
                [](auto value)
                { return nlohmann::json(value); },
                [](const Value::impl_type::string_type &boxed)
                { return nlohmann::json(boxed.get()); },
                [](const Value::impl_type::list_type &values)
                { return json_from(values); },
                [](const Value::impl_type::map_type &values)
                { return json_from(values); },
            },
            value.impl().variant);
    }
}

extern "C"
{
    traeger_format_t traeger_format_json{
        Format{
            traeger_string_t{String{"json"}},
            [](const Value &value) -> String
            {
                return json_from(value).dump();
            },
            [](const std::string_view &content) -> Value
            {
                return value_from_json(nlohmann::json::parse(content));
            }}};
}
