// SPDX-License-Identifier: BSL-1.0

#include "traeger/value/value.h"
#include "traeger/actor/actor.h"
#include "traeger/module/module.h"
#include "traeger/value/traeger_value.hpp"
#include "traeger/actor/traeger_actor.hpp"
#include "traeger/module/traeger_module.hpp"

extern "C"
{
    using namespace traeger;

    // Module

    bool traeger_module_new_from_path_or_error(const char *path_data,
                                               const size_t path_size,
                                               const traeger_map_t *init,
                                               traeger_module_t **result,
                                               traeger_string_t **error)
    {
        if (path_data != nullptr &&
            init != nullptr &&
            result != nullptr)
        {
            if (auto [module_optional, module_error] = Module::from_path(String{path_data, path_size},
                                                                         cast(init));
                module_optional)
            {
                *result = new traeger_module_t{std::move(module_optional).value()};
                return true;
            }
            else if (error != nullptr)
            {
                *error = new traeger_string_t{std::move(module_error)};
            }
        }
        return false;
    }

    traeger_module_t *traeger_module_copy(const traeger_module_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_module_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_module_free(traeger_module_t *self)
    {

        delete self;
    }

    traeger_mailbox_t *traeger_module_get_mailbox(const traeger_module_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_mailbox_t{cast(self).mailbox()};
        }
        return nullptr;
    }
}
