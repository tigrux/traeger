// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "traeger/format/format.h"
#include "traeger/format/Format.hpp"

namespace traeger
{
    inline auto cast(const traeger_format_t *format) noexcept -> const Format &
    {
        return *static_cast<const Format *>(format);
    }
}
