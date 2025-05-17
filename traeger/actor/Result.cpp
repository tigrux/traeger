// SPDX-License-Identifier: BSL-1.0

#include <utility>
#include <iomanip>
#include <ostream>

#include "traeger/actor/Result.hpp"

namespace traeger
{
    Result::Result() noexcept
        : type_(Type::Undefined)
    {
    }

    Result::Result(const Result &other) noexcept = default;

    Result::Result(Result &&other) noexcept
        : value_(std::move(other.value_)),
          type_(other.type_)
    {
        other.value_ = nullptr;
        other.type_ = Type::Undefined;
    }

    Result::Result(const Value &value) noexcept
    {
        value_ = value;
        type_ = Type::Value;
    }

    Result::Result(Value &&value) noexcept
    {
        value_ = std::move(value);
        type_ = Type::Value;
    }

    Result::Result(const Error &error) noexcept
    {
        value_ = error;
        type_ = Type::Error;
    }

    Result::Result(Error &&error) noexcept
    {
        value_ = std::move(error);
        type_ = Type::Error;
    }

    auto Result::operator=(const Result &other) noexcept -> Result & = default;

    auto Result::operator=(Result &&other) noexcept -> Result &
    {
        value_ = std::move(other.value_);
        type_ = other.type_;
        other.value_ = nullptr;
        other.type_ = Type::Undefined;
        return *this;
    }

    auto Result::operator==(const Result &other) const noexcept -> bool
    {
        return type_ == other.type_ && value_ == other.value_;
    }

    auto Result::operator!=(const Result &other) const noexcept -> bool
    {
        return type_ != other.type_ || value_ != other.value_;
    }

    auto Result::type() const noexcept -> Type
    {
        return type_;
    }

    auto Result::value() const & noexcept -> const Value *
    {
        if (type_ == Type::Value)
        {
            return &value_;
        }
        return nullptr;
    }

    auto Result::error() const & noexcept -> const String *
    {
        if (type_ == Type::Error)
        {
            return value_.get_string();
        }
        return nullptr;
    }

    auto Result::type_name() const noexcept -> const String &
    {
        return type_name(type_);
    }

    auto Result::type_name(Type type) noexcept -> const String &
    {
        static const String types[]{
            "Undefined",
            "Value",
            "Error",
        };
        return types[static_cast<int>(type)];
    }

    auto operator<<(std::ostream &os,
                    const Result &result) noexcept -> std::ostream &
    {
        const auto type = result.type();
        os << Result::type_name(type) << '(';
        switch (type)
        {
        case Result::Type::Undefined:
            break;
        case Result::Type::Value:
            os << *result.value();
            break;
        case Result::Type::Error:
            os << std::quoted(*result.error());
            break;
        }
        return os << ')';
    }
}
