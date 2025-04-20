// SPDX-License-Identifier: BSL-1.0

#include <variant>

#include <yaml-cpp/yaml.h>

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

    auto list_from_yaml(const YAML::Node &object) -> List;
    auto map_from_yaml(const YAML::Node &object) -> Map;
    auto value_from_yaml(const YAML::Node &object) -> Value;

    auto yaml_from(const Value::impl_type::list_type &values) -> YAML::Node;
    auto yaml_from(const Value::impl_type::map_type &values) -> YAML::Node;
    auto yaml_from(const Value &value) -> YAML::Node;

    auto list_from_yaml(const YAML::Node &object) -> List
    {
        List list;
        const auto begin = object.begin();
        const auto end = object.end();
        for (auto it = begin; it != end; ++it)
        {
            list.append(value_from_yaml(*it));
        }
        return list;
    }

    auto map_from_yaml(const YAML::Node &object) -> Map
    {
        Map map;
        const auto begin = object.begin();
        const auto end = object.end();
        for (auto it = object.begin(); it != end; ++it)
        {
            map.set(it->first.as<String>(), value_from_yaml(it->second));
        }
        return map;
    }

    auto value_from_yaml(const YAML::Node &object) -> Value
    {
        switch (object.Type())
        {
        default:
        case YAML::NodeType::value::Null:
            return nullptr;
        case YAML::NodeType::value::Scalar:
            return object.as<String>();
        case YAML::NodeType::value::Sequence:
            return list_from_yaml(object);
        case YAML::NodeType::value::Map:
            return map_from_yaml(object);
        }
    }

    auto yaml_from(const Value::impl_type::list_type &values) -> YAML::Node
    {
        YAML::Node object;
        for (const auto &value : values)
        {
            object.push_back(yaml_from(value));
        }
        return object;
    }

    auto yaml_from(const Value::impl_type::map_type &values) -> YAML::Node
    {
        YAML::Node object;
        for (const auto &[key, value] : values)
        {
            object[key] = yaml_from(value);
        }
        return object;
    }

    auto yaml_from(const Value &value) -> YAML::Node
    {
        return std::visit(
            overload{
                [](Null value)
                { return YAML::Node(); },
                [](auto value)
                { return YAML::Node(value); },
                [](const Value::impl_type::string_type &boxed)
                { return YAML::Node(boxed.get()); },
                [](const Value::impl_type::list_type &values)
                { return yaml_from(values); },
                [](const Value::impl_type::map_type &values)
                { return yaml_from(values); },
            },
            value.impl().variant);
    }
}

extern "C"
{
    traeger_format_t traeger_format_yaml{
        Format{
            "yaml",
            [](const Value &value) -> String
            {
                return YAML::Dump(yaml_from(value));
            },
            [](const std::string_view &content) -> Value
            {
                return value_from_yaml(YAML::Load(String{content}));
            }}};
}
