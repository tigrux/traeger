// SPDX-License-Identifier: BSL-1.0

#include "traeger.hpp"

NB_MODULE(traeger, module)
{
    traeger_python_register_value_types(module);
    traeger_python_register_format_types(module);
    traeger_python_register_actor_types(module);
    traeger_python_register_socket_types(module);
}
