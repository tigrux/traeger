// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/value/value.h>

typedef struct traeger_format_t traeger_format_t;

#ifdef __cplusplus
extern "C"
{
#endif

    const traeger_format_t *traeger_format_get_by_name(const char *name_data,
                                                       size_t name_size);

    const char *traeger_format_get_name(const traeger_format_t *self);

    bool traeger_format_encode_value(const traeger_format_t *self,
                                     const traeger_value_t *value,
                                     traeger_string_t **result,
                                     traeger_string_t **error);

    bool traeger_format_decode_string(const traeger_format_t *self,
                                      const traeger_string_t *string,
                                      traeger_value_t **result,
                                      traeger_string_t **error);

#ifdef __cplusplus
}
#endif
