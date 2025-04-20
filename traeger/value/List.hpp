// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <exception>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

#include <traeger/value/Types.hpp>

namespace traeger
{
    struct List
    {
        struct impl_type;

        inline auto impl() const & noexcept -> const impl_type &
        {
            return *reinterpret_cast<const impl_type *>(impl_);
        }

        inline auto impl() & noexcept -> impl_type &
        {
            return *reinterpret_cast<impl_type *>(impl_);
        }

        ~List() noexcept;

        List() noexcept;

        List(const List &other) noexcept;

        List(List &&list) noexcept;

        explicit List(const impl_type &impl) noexcept;

        explicit List(impl_type &&impl) noexcept;

        auto operator=(const List &other) noexcept -> List &;

        auto operator=(List &&other) noexcept -> List &;

        auto append(const Value &value) noexcept -> void;

        auto append(Value &&value) noexcept -> void;

        auto set(int index,
                 const Value &value) noexcept -> bool;

        auto set(int index,
                 Value &&value) noexcept -> bool;

        auto find(int index) const noexcept -> const Value *;

        auto empty() const noexcept -> bool;

        auto size() const noexcept -> std::size_t;

        auto resize(std::size_t new_size) noexcept -> std::size_t;

        template <typename... Args>
        auto get_tuple() const -> std::tuple<Args...>
        {
            constexpr auto n_args = sizeof...(Args);
            if (n_args != size())
            {
                throw std::runtime_error(
                    "expected " + std::to_string(n_args) +
                    " arguments but " + std::to_string(size()) +
                    " were given");
            }
            return get_index_sequence<std::tuple<Args...>>(std::make_integer_sequence<int, n_args>());
        }

        template <typename... Args>
        auto get(Args &...args) const -> std::pair<bool, String>
        {
            try
            {
                std::tie(args...) = get_tuple<Args...>();
                return {true, String{}};
            }
            catch (const std::exception &e)
            {
                return {false, e.what()};
            }
        }

        template <typename... Args>
        void append(const Value &value, Args &&...args) noexcept
        {
            append(value);
            append(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void append(Value &&value, Args &&...args) noexcept
        {
            append(std::move(value));
            append(std::forward<Args>(args)...);
        }

        struct Iterator
        {
            struct impl_type;

            inline auto impl() const & noexcept -> const List::Iterator::impl_type &
            {
                return *reinterpret_cast<const impl_type *>(impl_);
            }

            inline auto impl() & noexcept -> List::Iterator::impl_type &
            {
                return *reinterpret_cast<impl_type *>(impl_);
            }

            ~Iterator() noexcept;

            Iterator(const List &list) noexcept;

            operator bool() const noexcept;

            auto operator!=(bool end) noexcept -> bool;

            auto operator==(bool end) noexcept -> bool;

            auto value() const noexcept -> const Value &;
            auto operator*() const noexcept -> const Value &;

            auto increment() noexcept -> bool;
            auto operator++() noexcept -> Iterator &;

            using layout_type = struct
            {
                struct
                {
                    std::uintptr_t _0;
                    std::size_t _1;
                    std::uintptr_t _2;
                    std::uintptr_t _3;
                } _0, _1;
            };

        private:
            std::byte impl_[sizeof(layout_type)];
        };

        auto begin() const noexcept -> Iterator;
        auto end() const noexcept -> bool;

        using layout_type = struct
        {
            std::size_t _0;
            std::uint32_t _1;
            std::uintptr_t _2;
            std::uintptr_t _3;
        };

    private:
        template <int Index, typename Arg>
        auto get_index_arg() const -> Arg;

        template <typename Tuple, int... index>
        auto get_index_sequence(std::integer_sequence<int, index...>) const -> Tuple
        {
            return std::make_tuple(get_index_arg<index, typename std::tuple_element<index, Tuple>::type>()...);
        }

        std::byte impl_[sizeof(layout_type)];
    };

    auto operator<<(std::ostream &os,
                    const List &list) noexcept -> std::ostream &;

    template <typename... Args>
    auto make_list(Args &&...args) noexcept -> List
    {
        List list;
        if constexpr (sizeof...(args) > 0)
        {
            list.append(std::forward<Args>(args)...);
        }
        return list;
    }

}

inline auto begin(const traeger::List &list) -> traeger::List::Iterator
{
    return list.begin();
}

inline auto end(const traeger::List &list) -> bool
{
    return list.end();
}

struct traeger_list_t : traeger::List
{
};

struct traeger_list_iterator_t : traeger::List::Iterator
{
};
