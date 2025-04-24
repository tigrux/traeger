// SPDX-License-Identifier: BSL-1.0

#include <utility>
#include <optional>
#include <cstddef>
#include <ostream>

#include "traeger/value/Value.hpp"
#include "traeger/value/List_impl.hpp"

namespace traeger
{
    List::impl_type::~impl_type() noexcept
    {
    }

    List::impl_type::impl_type() noexcept
        : list()
    {
    }

    List::impl_type::impl_type(const impl_type &other) noexcept
        : list(other.list)
    {
    }

    List::impl_type::impl_type(impl_type &&other) noexcept
        : list(std::move(other.list))
    {
    }

    List::impl_type::impl_type(const persistent_type &persistent) noexcept
        : list(persistent.transient())
    {
    }

    List::impl_type::impl_type(persistent_type &&persistent) noexcept
        : list(std::move(persistent).transient())
    {
    }

    auto List::impl_type::persistent() const & noexcept -> persistent_type
    {
        auto temp = list;
        return std::move(temp).persistent();
    }

    auto List::impl_type::persistent() && noexcept -> persistent_type
    {
        return std::move(list).persistent();
    }

    List::~List() noexcept
    {
        impl().~impl_type();
    }

    List::List() noexcept
    {
        new (impl_) impl_type{};
    }

    List::List(const List &other) noexcept
        : List()
    {
        impl().list = other.impl().list;
    }

    List::List(List &&other) noexcept
        : List()
    {
        impl().list = std::move(other.impl().list);
    }

    List::List(const impl_type &other_impl) noexcept
        : List()
    {
        impl().list = other_impl.list;
    }

    List::List(impl_type &&other_impl) noexcept
        : List()
    {
        impl().list = std::move(other_impl.list);
    }

    auto List::operator=(const List &other) noexcept -> List &
    {
        impl().list = other.impl().list;
        return *this;
    }

    auto List::operator=(List &&other) noexcept -> List &
    {
        impl().list = std::move(other.impl().list);
        return *this;
    }

    auto List::operator==(const List &other) const noexcept -> bool
    {
        return impl().persistent() == other.impl().persistent();
    }

    auto List::operator!=(const List &other) const noexcept -> bool
    {
        return impl().persistent() != other.impl().persistent();
    }

    auto List::append(const Value &value) noexcept -> void
    {
        impl().list.push_back(value);
    }

    auto List::append(Value &&value) noexcept -> void
    {
        impl().list.push_back(std::move(value));
    }

    auto List::set(int index,
                   const Value &value) noexcept -> bool
    {
        const auto size_ = size();
        const std::size_t position = (index < 0) ? index + size_ : index;
        if (position < size_)
        {
            impl().list.set(position, value);
            return true;
        }
        return false;
    }

    auto List::set(int index,
                   Value &&value) noexcept -> bool
    {
        const auto size_ = size();
        const std::size_t position = (index < 0) ? index + size_ : index;
        if (position < size_)
        {
            impl().list.set(position, std::move(value));
            return true;
        }
        return false;
    }

    auto List::find(int index) const noexcept -> const Value *
    {
        const auto size_ = size();
        if (const std::size_t position = (index < 0) ? index + size_ : index; position < size_)
        {
            return &impl().list[position].get();
        }
        return nullptr;
    }

    auto List::empty() const noexcept -> bool
    {
        return impl().list.empty();
    }

    auto List::size() const noexcept -> std::size_t
    {
        return impl().list.size();
    }

    auto List::resize(std::size_t new_size) noexcept -> std::size_t
    {
        const auto size_ = size();
        if (new_size < size_)
        {
            impl().list.take(new_size);
        }
        else if (new_size > size_)
        {
            for (auto n = size_; n < new_size; ++n)
            {
                impl().list.push_back(nullptr);
            }
        }
        return size();
    }

    List::Iterator::impl_type::~impl_type() noexcept
    {
    }

    List::Iterator::impl_type::impl_type(const iterator_type &begin, const iterator_type &end) noexcept
        : begin(begin),
          end(end)
    {
    }

    List::Iterator::impl_type::impl_type(const impl_type &other) noexcept
        : begin(other.begin),
          end(other.end)
    {
    }

    List::Iterator::impl_type::impl_type(impl_type &&other) noexcept
        : begin(std::move(other.begin)),
          end(std::move(other.end))
    {
    }

    List::Iterator::~Iterator() noexcept
    {
        impl().~impl_type();
    }

    List::Iterator::Iterator(const List &list) noexcept
    {
        new (impl_) impl_type{list.impl().list.begin(), list.impl().list.end()};
    }

    List::Iterator::operator bool() const noexcept
    {
        return (impl().begin != impl().end);
    }

    auto List::Iterator::value() const noexcept -> const Value &
    {
        return impl().begin->get();
    }

    auto List::Iterator::operator*() const noexcept -> const Value &
    {
        return impl().begin->get();
    }

    auto List::Iterator::increment() noexcept -> bool
    {
        if (*this)
        {
            ++impl().begin;
            return bool(*this);
        }
        return false;
    }

    auto List::Iterator::operator++() noexcept -> List::Iterator &
    {
        increment();
        return *this;
    }

    auto List::Iterator::operator!=(bool end) noexcept -> bool
    {
        return bool(*this) != end;
    }

    auto List::Iterator::operator==(bool end) noexcept -> bool
    {
        return bool(*this) == end;
    }

    auto List::begin() const noexcept -> List::Iterator
    {
        return Iterator{*this};
    }

    auto List::end() const noexcept -> bool
    {
        return false;
    }

    auto operator<<(std::ostream &os,
                    const List &list) noexcept -> std::ostream &
    {
        auto iter = list.begin();
        os << '[';
        for (bool is_first = true; iter; ++iter)
        {
            (is_first ? (is_first = false, os) : os << ", ")
                << iter.value();
        }
        os << ']';
        return os;
    }
}
