// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <string>

#include <traeger/value/types.h>

namespace traeger
{
    using Null = std::nullptr_t;
    using Bool = traeger_bool_t;
    using Int = traeger_int_t;
    using UInt = traeger_uint_t;
    using Float = traeger_float_t;
    using String = std::string;
    struct List;
    struct Map;
    struct Value;
}

struct traeger_string_t : traeger::String
{
};
