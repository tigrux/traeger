// SPDX-License-Identifier: BSL-1.0

#include <iostream>

#include <traeger/value/Value.hpp>
#include <traeger/actor/Actor.hpp>

#include <traeger/module/module.h>

extern traeger::Actor make_account_actor(traeger::Float initial_funds);

extern "C" DLLEXPORT void
traeger_module_init(const traeger_map_t *init,
                    traeger_mailbox_interface_t **result,
                    traeger_string_t *error)
{
    if (init == nullptr ||
        result == nullptr ||
        error == nullptr)
    {
        return;
    }

    traeger::Float initial_funds = 0.0;
    init->get("initial_funds", initial_funds);
    std::cout << "initial_funds: " << initial_funds << std::endl;

    const auto actor = make_account_actor(initial_funds);
    *result = actor.mailbox_interface().release();
}
