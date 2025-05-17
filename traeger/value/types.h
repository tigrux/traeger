// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum traeger_value_type_t
{
    TRAEGER_VALUE_TYPE_NULL = 0,
    TRAEGER_VALUE_TYPE_BOOL = 1,
    TRAEGER_VALUE_TYPE_INT = 2,
    TRAEGER_VALUE_TYPE_UINT = 3,
    TRAEGER_VALUE_TYPE_FLOAT = 4,
    TRAEGER_VALUE_TYPE_STRING = 5,
    TRAEGER_VALUE_TYPE_LIST = 6,
    TRAEGER_VALUE_TYPE_MAP = 7,
} traeger_value_type_t;

typedef bool traeger_bool_t;

typedef int64_t traeger_int_t;

typedef uint64_t traeger_uint_t;

typedef double traeger_float_t;
