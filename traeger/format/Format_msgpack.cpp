// SPDX-License-Identifier: BSL-1.0

#include <variant>
#include <string>
#include <utility>
#include <cstddef>

#include <msgpack.hpp>

#include "traeger/value/Value_impl.hpp"
#include "traeger/format/Format.hpp"

namespace
{
    using namespace traeger;

    struct Stream
    {
        std::string str;

        auto write(const char *data, std::size_t size) noexcept -> void
        {
            str.append(data, size);
        }
    };

    using Packer = msgpack::packer<Stream>;

    template <class... Types>
    struct overload : Types...
    {
        using Types::operator()...;
    };

    template <class... Types>
    overload(Types...) -> overload<Types...>;

    auto list_from_msgpack(const msgpack::object &object) -> List;
    auto map_from_msgpack(const msgpack::object &object) -> Map;
    auto value_from_msgpack(const msgpack::object &object) -> Value;

    auto pack(Packer &packer, const Value::impl_type::list_type &values) -> void;
    auto pack(Packer &packer, const Value::impl_type::map_type &values) -> void;
    auto pack(Packer &packer, const Value &value) -> void;

    auto list_from_msgpack(const msgpack::object &object) -> List
    {
        List list;
        const auto begin = object.via.array.ptr;
        const auto end = begin + object.via.array.size;
        for (auto iter = begin; iter != end; ++iter)
        {
            list.append(value_from_msgpack(*iter));
        }
        return list;
    }

    auto map_from_msgpack(const msgpack::object &object) -> Map
    {
        Map map;
        const auto begin = object.via.map.ptr;
        const auto end = begin + object.via.map.size;
        for (auto iter = begin; iter != end; ++iter)
        {
            map.set(String{iter->key.via.str.ptr, iter->key.via.str.size},
                    value_from_msgpack(iter->val));
        }
        return map;
    }

    auto value_from_msgpack(const msgpack::object &object) -> Value
    {
        switch (object.type)
        {
        default:
        case msgpack::type::NIL:
            return nullptr;
        case msgpack::type::BOOLEAN:
            return object.via.boolean;
        case msgpack::type::POSITIVE_INTEGER:
            return object.via.u64;
        case msgpack::type::NEGATIVE_INTEGER:
            return object.via.i64;
        case msgpack::type::FLOAT32:
        case msgpack::type::FLOAT64:
            return object.via.f64;
        case msgpack::type::STR:
            return String(object.via.str.ptr, object.via.str.size);
        case msgpack::type::ARRAY:
            return list_from_msgpack(object);
        case msgpack::type::MAP:
            return map_from_msgpack(object);
        }
    }

    auto pack(Packer &packer,
              const Value::impl_type::list_type &values) -> void
    {
        packer.pack_array(values.size());
        for (const auto &value : values)
        {
            pack(packer, value.get());
        }
    }

    auto pack(Packer &packer,
              const Value::impl_type::map_type &values) -> void
    {
        packer.pack_map(values.size());
        for (const auto &[key, value] : values)
        {
            packer.pack(key);
            pack(packer, value.get());
        }
    }

    auto pack(Packer &packer,
              const Value &value) -> void
    {
        std::visit(
            overload{
                [&packer](Null value)
                { packer.pack_nil(); },
                [&packer](auto value)
                { packer.pack(value); },
                [&packer](const Value::impl_type::string_type &boxed)
                { packer.pack(boxed.get()); },
                [&packer](const Value::impl_type::list_type &values)
                { pack(packer, values); },
                [&packer](const Value::impl_type::map_type &values)
                { pack(packer, values); },
            },
            value.impl().variant);
    }
}

extern "C"
{
    traeger_format_t traeger_format_msgpack{
        Format{
            "msgpack",
            [](const Value &value) -> String
            {
                Stream stream;
                Packer packer{stream};
                pack(packer, value);
                return std::move(stream.str);
            },
            [](const std::string_view &content) -> Value
            {
                auto pack = msgpack::unpack(content.data(), content.size());
                return value_from_msgpack(pack.get());
            }}};
}
