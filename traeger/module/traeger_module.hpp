// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "traeger/module/module.h"
#include "traeger/module/Module.hpp"

namespace traeger
{
    inline auto cast(const traeger_module_t *module) -> const Module &
    {
        return *static_cast<const Module *>(module);
    }
}
