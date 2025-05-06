// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>

#include <traeger/value/Types.hpp>

namespace traeger
{
    struct Map
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

        ~Map() noexcept;

        Map() noexcept;

        Map(const Map &other) noexcept;

        Map(Map &&other) noexcept;

        explicit Map(const impl_type &impl) noexcept;

        explicit Map(impl_type &&impl) noexcept;

        auto operator=(const Map &other) noexcept -> Map &;

        auto operator=(Map &&other) noexcept -> Map &;

        auto operator==(const Map &other) const noexcept -> bool;

        auto operator!=(const Map &other) const noexcept -> bool;

        auto set(const String &key,
                 const Value &value) noexcept -> void;

        auto set(const String &key,
                 Value &&value) noexcept -> void;

        auto erase(const String &key) noexcept -> void;

        auto contains(const String &key) const noexcept -> bool;

        auto find(const String &key) const noexcept -> const Value *;

        auto empty() const noexcept -> bool;

        auto size() const noexcept -> std::size_t;

        template <typename... Args>
        auto set(const String &key, const Value &value, Args &&...args) noexcept -> void
        {
            set(key, value);
            if constexpr (sizeof...(args) > 0)
            {
                set(std::forward<Args>(args)...);
            }
        }

        template <typename... Args>
        auto set(const String &key, Value &&value, Args &&...args) noexcept -> void
        {
            set(key, std::move(value));
            if constexpr (sizeof...(args) > 0)
            {
                set(std::forward<Args>(args)...);
            }
        }

        template <typename Variant, typename... Args>
        auto get(const String &key, Variant &variant) const noexcept -> std::pair<bool, String>;

        template <typename Variant, typename... Args>
        auto get(const String &key, Variant &variant, Args &&...args) const noexcept -> std::pair<bool, String>;

        struct Iterator
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

            ~Iterator() noexcept;

            Iterator(const Map &map) noexcept;

            operator bool() const noexcept;
            auto operator!=(bool end) noexcept -> bool;
            auto operator==(bool end) noexcept -> bool;

            auto key() const noexcept -> const String &;
            auto value() const noexcept -> const Value &;
            auto operator*() const noexcept -> std::pair<const String &, const Value &>;

            auto increment() noexcept -> bool;
            auto operator++() noexcept -> Iterator &;

            using layout_type = struct
            {
                struct
                {
                    std::uintptr_t _0;
                    std::uintptr_t _1;
                    std::uint32_t _2;
                    std::uintptr_t _3[1 + (sizeof(std::size_t) * 8 + 5 - 1) / 5];
                } _0, _1;
            };

        private:
            std::byte impl_[sizeof(layout_type)];
        };

        auto begin() const noexcept -> Iterator;
        auto end() const noexcept -> bool;

        using layout_type = struct
        {
            std::uintptr_t _0;
            std::size_t _1;
        };

    private:
        std::byte impl_[sizeof(layout_type)];
    };

    auto operator<<(std::ostream &os,
                    const Map &map) noexcept -> std::ostream &;

    template <typename... Args>
    auto make_map(Args &&...args) noexcept -> Map
    {
        Map map;
        if constexpr (sizeof...(args) > 0)
        {
            map.set(std::forward<Args>(args)...);
        }
        return map;
    }
}

inline auto begin(const traeger::Map &map) -> traeger::Map::Iterator
{
    return map.begin();
}

inline auto end(const traeger::Map &map) -> bool
{
    return map.end();
}

struct traeger_map_t final : traeger::Map
{
};

struct traeger_map_iterator_t final : traeger::Map::Iterator
{
};
