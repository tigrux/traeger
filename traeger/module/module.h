// SPDX-License-Identifier: BSL-1.0

#pragma once

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <traeger/value/value.h>
#include <traeger/actor/actor.h>

typedef void (*traeger_module_init_t)(const traeger_map_t *init,
                                      traeger_mailbox_interface_t **result,
                                      traeger_string_t *error);

typedef struct traeger_module_t traeger_module_t;

#ifdef __cplusplus
extern "C"
{
#endif

    DLLEXPORT void
    traeger_module_init(const traeger_map_t *init,
                        traeger_mailbox_interface_t **result,
                        traeger_string_t *error);

    // Module

    bool traeger_module_new_from_path_or_error(const char *path_data,
                                               size_t path_size,
                                               const traeger_map_t *init,
                                               traeger_module_t **result,
                                               traeger_string_t **error);

    traeger_module_t *traeger_module_copy(const traeger_module_t *self);

    void traeger_module_free(traeger_module_t *self);

    traeger_mailbox_t *traeger_module_get_mailbox(const traeger_module_t *self);

#ifdef __cplusplus
}
#endif
