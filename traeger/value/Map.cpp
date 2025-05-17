// SPDX-License-Identifier: BSL-1.0

#include <iomanip>
#include <ostream>
#include <utility>

#include "traeger/value/Value.hpp"
#include "traeger/value/Map_impl.hpp"

namespace traeger
{
    Map::impl_type::impl_type(const persistent_type &persistent) noexcept
        : map(persistent.transient())
    {
    }

    Map::impl_type::impl_type(persistent_type &&persistent) noexcept
        : map(std::move(persistent).transient())
    {
    }

    auto Map::impl_type::persistent() const & noexcept -> persistent_type
    {
        auto temp = map;
        return std::move(temp).persistent();
    }

    auto Map::impl_type::persistent() && noexcept -> persistent_type
    {
        return std::move(map).persistent();
    }

    Map::~Map() noexcept
    {
        impl().~impl_type();
    }

    Map::Map() noexcept
    {
        new (impl_) impl_type{};
    }

    Map::Map(const Map &other) noexcept
        : Map()
    {
        impl().map = other.impl().map;
    }

    Map::Map(Map &&other) noexcept
        : Map()
    {
        impl().map = std::move(other.impl().map);
    }

    Map::Map(const impl_type &other_impl) noexcept
        : Map()
    {
        impl().map = other_impl.map;
    }

    Map::Map(impl_type &&other_impl) noexcept
        : Map()
    {
        impl().map = std::move(other_impl.map);
    }

    auto Map::operator=(const Map &other) noexcept -> Map &
    {
        if (this != &other)
        {
            impl().map = other.impl().map;
        }
        return *this;
    }

    auto Map::operator=(Map &&other) noexcept -> Map &
    {
        impl().map = std::move(other.impl().map);
        return *this;
    }

    auto Map::operator==(const Map &other) const noexcept -> bool
    {
        return impl().persistent() == other.impl().persistent();
    }

    auto Map::operator!=(const Map &other) const noexcept -> bool
    {
        return impl().persistent() != other.impl().persistent();
    }

    auto Map::set(const String &key,
                  const Value &value) noexcept -> void
    {
        impl().map.set(key, value);
    }

    auto Map::set(const String &key,
                  Value &&value) noexcept -> void
    {
        impl().map.set(key, std::move(value));
    }

    auto Map::erase(const String &key) noexcept -> void
    {
        impl().map.erase(key);
    }

    auto Map::contains(const String &key) const noexcept -> bool
    {
        return impl().map.find(key) != nullptr;
    }

    auto Map::find(const String &key) const noexcept -> const Value *
    {
        if (const auto *value_ptr = impl().map.find(key); value_ptr)
        {
            return &value_ptr->get();
        }
        return nullptr;
    }

    auto Map::empty() const noexcept -> bool
    {
        return impl().map.empty();
    }

    auto Map::size() const noexcept -> std::size_t
    {
        return impl().map.size();
    }

    Map::Iterator::impl_type::impl_type(const iterator_type &begin, const iterator_type &end) noexcept
        : begin(begin),
          end(end)
    {
    }

    Map::Iterator::~Iterator() noexcept
    {
        impl().~impl_type();
    }

    Map::Iterator::Iterator(const Map &map) noexcept
    {
        new (impl_) impl_type{map.impl().map.begin(), map.impl().map.end()};
    }

    Map::Iterator::operator bool() const noexcept
    {
        return impl().begin != impl().end;
    }

    auto Map::Iterator::key() const noexcept -> const String &
    {
        return impl().begin->first;
    }

    auto Map::Iterator::value() const noexcept -> const Value &
    {
        return impl().begin->second.get();
    }

    auto Map::Iterator::operator*() const noexcept -> std::pair<const String &, const Value &>
    {
        return {key(), value()};
    }

    auto Map::Iterator::increment() noexcept -> bool
    {
        if (*this)
        {
            ++impl().begin;
            return static_cast<bool>(*this);
        }
        return false;
    }

    auto Map::Iterator::operator++() noexcept -> Iterator &
    {
        increment();
        return *this;
    }

    auto Map::Iterator::operator!=(const bool end) const noexcept -> bool
    {
        return static_cast<bool>(*this) != end;
    }

    auto Map::Iterator::operator==(const bool end) const noexcept -> bool
    {
        return static_cast<bool>(*this) == end;
    }

    auto Map::begin() const noexcept -> Iterator
    {
        return Iterator{*this};
    }

    auto Map::end() noexcept -> bool
    {
        return false;
    }

    auto operator<<(std::ostream &os,
                    const Map &map) noexcept -> std::ostream &
    {
        auto iter = map.begin();
        os << '{';
        for (bool is_first = true; iter; ++iter)
        {
            (is_first ? (is_first = false, os) : os << ", ")
                << std::quoted(iter.key())
                << ':' << iter.value();
        }
        os << '}';
        return os;
    }
}
