// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <immer/box.hpp>
#include <immer/map.hpp>
#include <immer/map_transient.hpp>

#include "traeger/value/Map.hpp"

namespace traeger
{
    struct Map::impl_type
    {
        using key_type = String;
        using value_type = immer::box<Value>;
        using persistent_type = immer::map<key_type, value_type>;
        using transient_type = immer::map_transient<key_type, value_type>;

        ~impl_type() noexcept = default;

        impl_type() = default;

        impl_type(const impl_type &other) noexcept = default;

        impl_type(impl_type &&other) noexcept = default;

        explicit impl_type(const persistent_type &persistent) noexcept;

        explicit impl_type(persistent_type &&persistent) noexcept;

        auto persistent() const & noexcept -> persistent_type;

        auto persistent() && noexcept -> persistent_type;

        static_assert(sizeof(layout_type) == sizeof(transient_type));

        transient_type map;
    };

    struct Map::Iterator::impl_type
    {
        ~impl_type() noexcept = default;

        using iterator_type = Map::impl_type::transient_type::iterator;

        impl_type(const iterator_type &begin, const iterator_type &end) noexcept;

        impl_type(const impl_type &other) noexcept = default;

        impl_type(impl_type &&other) noexcept = default;

        static_assert(sizeof(layout_type) == 2 * sizeof(iterator_type));

        iterator_type begin, end;
    };
}
