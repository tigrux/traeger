// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <exception>
#include <functional>
#include <ostream>
#include <type_traits>
#include <utility>

#include <traeger/value/Value.hpp>

namespace traeger
{
    struct Error : String
    {
    };

    struct Result
    {
        enum class Type : int
        {
            Undefined = 0,
            Value = 1,
            Error = 2,
        };

        Result() noexcept;

        Result(const Result &other) noexcept;

        Result(Result &&other) noexcept;

        Result(const Value &value) noexcept;

        Result(Value &&value) noexcept;

        Result(const Error &error) noexcept;

        Result(Error &&error) noexcept;

        Result &operator=(const Result &other) noexcept;

        Result &operator=(Result &&other) noexcept;

        auto operator==(const Result &other) const noexcept -> bool;

        auto operator!=(const Result &other) const noexcept -> bool;

        auto type() const noexcept -> Type;

        auto value() const & noexcept -> const Value *;

        auto error() const & noexcept -> const String *;

        auto type_name() noexcept -> const String &;

        static auto type_name(Type type) noexcept -> const String &;

    private:
        Value value_;
        Type type_;
    };

    auto operator<<(std::ostream &os,
                    const Result &result) noexcept -> std::ostream &;
}

struct traeger_result_t : traeger::Result
{
};
