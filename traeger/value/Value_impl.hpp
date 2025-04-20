// SPDX-License-Identifier: BSL-1.0

#include "traeger/value/Value.hpp"
#include "traeger/value/List_impl.hpp"
#include "traeger/value/Map_impl.hpp"

namespace traeger
{
    struct Value::impl_type
    {
        using string_type = immer::box<String>;
        using list_type = List::impl_type::persistent_type;
        using map_type = Map::impl_type::persistent_type;

        using variant_type = std::variant<
            Null,
            Bool,
            Int,
            UInt,
            Float,
            string_type,
            list_type,
            map_type>;

        ~impl_type() noexcept;

        impl_type() noexcept;

        impl_type(const impl_type &other) noexcept;

        impl_type(impl_type &&other) noexcept;

        impl_type(const variant_type &variant) noexcept;

        impl_type(variant_type &&variant) noexcept;

        auto get_list() const noexcept -> const list_type *;

        auto get_map() const noexcept -> const map_type *;

        auto get_string() const noexcept -> const string_type *;

        static_assert(sizeof(layout_type) == sizeof(variant_type));

        variant_type variant;
    };
}
