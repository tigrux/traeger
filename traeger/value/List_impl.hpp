// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <immer/box.hpp>
#include <immer/vector.hpp>
#include <immer/vector_transient.hpp>

#include "traeger/value/List.hpp"

namespace traeger
{
    struct List::impl_type
    {
        using value_type = immer::box<Value>;
        using persistent_type = immer::vector<value_type>;
        using transient_type = immer::vector_transient<value_type>;

        ~impl_type() noexcept;

        impl_type() noexcept;

        impl_type(const impl_type &other) noexcept;

        impl_type(impl_type &&other) noexcept;

        impl_type(const persistent_type &persistent) noexcept;

        impl_type(persistent_type &&persistent) noexcept;

        auto persistent() const & noexcept -> persistent_type;

        auto persistent() && noexcept -> persistent_type;

        static_assert(sizeof(layout_type) == sizeof(transient_type));

        transient_type list;
    };

    struct List::Iterator::impl_type
    {
        using iterator_type = List::impl_type::transient_type::iterator;

        ~impl_type() noexcept;

        impl_type(const iterator_type &begin, const iterator_type &end) noexcept;

        impl_type(const impl_type &other) noexcept;

        impl_type(impl_type &&other) noexcept;

        static_assert(sizeof(layout_type) == 2 * sizeof(iterator_type));

        iterator_type begin, end;
    };
}
