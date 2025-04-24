// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "traeger/value/value.h"
#include "traeger/value/Value.hpp"

namespace traeger
{
    inline auto cast(const traeger_string_t *string) noexcept -> const std::string &
    {
        return *static_cast<const std::string *>(string);
    }

    inline auto cast(traeger_list_t *list) noexcept -> List &
    {
        return *static_cast<List *>(list);
    }

    inline auto cast(const traeger_list_t *list) noexcept -> const List &
    {
        return *static_cast<const List *>(list);
    }

    inline auto cast(traeger_map_t *map) noexcept -> Map &
    {
        return *static_cast<Map *>(map);
    }

    inline auto cast(const traeger_map_t *map) noexcept -> const Map &
    {
        return *static_cast<const Map *>(map);
    }

    inline auto cast(traeger_value_t *value) noexcept -> Value &
    {
        return *static_cast<Value *>(value);
    }

    inline auto cast(const traeger_value_t *value) noexcept -> const Value &
    {
        return *static_cast<const Value *>(value);
    }

    inline auto cast(traeger_list_iterator_t *iterator) noexcept -> List::Iterator &
    {
        return *static_cast<List::Iterator *>(iterator);
    }

    inline auto cast(const traeger_list_iterator_t *iterator) noexcept -> const List::Iterator &
    {
        return *static_cast<const List::Iterator *>(iterator);
    }

    inline auto cast(traeger_map_iterator_t *iterator) noexcept -> Map::Iterator &
    {
        return *static_cast<Map::Iterator *>(iterator);
    }

    inline auto cast(const traeger_map_iterator_t *iterator) noexcept -> const Map::Iterator &
    {
        return *static_cast<const Map::Iterator *>(iterator);
    }

    inline auto cast(Value::Type type) noexcept -> traeger_value_type_t
    {
        const auto type_as_int = static_cast<int>(type);
        return static_cast<traeger_value_type_t>(type_as_int);
    }
}
