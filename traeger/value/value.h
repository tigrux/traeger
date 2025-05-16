// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/value/types.h>

typedef struct traeger_string_t traeger_string_t;

typedef const traeger_string_t traeger_const_string_t;

typedef struct traeger_list_t traeger_list_t;

typedef const traeger_list_t traeger_const_list_t;

typedef struct traeger_list_iterator_t traeger_list_iterator_t;

typedef struct traeger_map_t traeger_map_t;

typedef const struct traeger_map_t traeger_const_map_t;

typedef struct traeger_map_iterator_t traeger_map_iterator_t;

typedef struct traeger_value_t traeger_value_t;

typedef const traeger_value_t traeger_const_value_t;

typedef struct traeger_result_t traeger_result_t;

#ifdef __cplusplus
extern "C"
{
#endif

    // String

    traeger_string_t *traeger_string_new(const char *string_data,
                                         size_t string_size);

    traeger_string_t *traeger_string_copy(const traeger_string_t *self);

    void traeger_string_free(traeger_string_t *self);

    void traeger_string_set(traeger_string_t *self,
                            const char *string_data,
                            size_t string_size);

    const char *traeger_string_data(const traeger_string_t *self);

    size_t traeger_string_size(const traeger_string_t *self);

    bool traeger_string_equal(const traeger_string_t *self,
                              const traeger_string_t *other);

    // List

    traeger_list_t *traeger_list_new();

    traeger_list_t *traeger_list_copy(const traeger_list_t *self);

    void traeger_list_free(traeger_list_t *self);

    void traeger_list_print(const traeger_list_t *self);

    void traeger_list_to_string(const traeger_list_t *self,
                                traeger_string_t **string);

    bool traeger_list_equal(const traeger_list_t *self,
                            const traeger_list_t *other);

    void traeger_list_append_value(traeger_list_t *self,
                                   const traeger_value_t *value);

    void traeger_list_append_null(traeger_list_t *self);

    void traeger_list_append_bool(traeger_list_t *self,
                                  traeger_bool_t value);

    void traeger_list_append_int(traeger_list_t *self,
                                 traeger_int_t value);

    void traeger_list_append_uint(traeger_list_t *self,
                                  traeger_uint_t value);

    void traeger_list_append_float(traeger_list_t *self,
                                   traeger_float_t value);

    void traeger_list_append_string(traeger_list_t *self,
                                    const char *string_data,
                                    size_t string_size);

    void traeger_list_append_list(traeger_list_t *self,
                                  const traeger_list_t *list);

    void traeger_list_append_map(traeger_list_t *self,
                                 const traeger_map_t *map);

    void traeger_list_set_value(traeger_list_t *self,
                                int index,
                                const traeger_value_t *value);

    void traeger_list_set_null(traeger_list_t *self,
                               int index);

    void traeger_list_set_bool(traeger_list_t *self,
                               int index,
                               traeger_bool_t value);

    void traeger_list_set_int(traeger_list_t *self,
                              int index,
                              traeger_int_t value);

    void traeger_list_set_uint(traeger_list_t *self,
                               int index,
                               traeger_uint_t value);

    void traeger_list_set_float(traeger_list_t *self,
                                int index,
                                traeger_float_t value);

    void traeger_list_set_string(traeger_list_t *self,
                                 int index,
                                 const char *string_data,
                                 size_t string_size);

    void traeger_list_set_list(traeger_list_t *self,
                               int index,
                               const traeger_list_t *list);

    void traeger_list_set_map(traeger_list_t *self,
                              int index,
                              const traeger_map_t *map);

    bool traeger_list_find(const traeger_list_t *self,
                           int index,
                           traeger_value_t **value);

    bool traeger_list_empty(const traeger_list_t *self);

    size_t traeger_list_size(const traeger_list_t *self);

    size_t traeger_list_resize(traeger_list_t *self,
                               size_t new_size);

    // List::Iterator

    traeger_list_iterator_t *traeger_list_iterator_new(const traeger_list_t *self);

    bool traeger_list_iterator_has_next(const traeger_list_iterator_t *self);

    bool traeger_list_iterator_next(traeger_list_iterator_t *self,
                                    traeger_value_t **value);

    void traeger_list_iterator_free(traeger_list_iterator_t *self);

    // Map

    traeger_map_t *traeger_map_new();

    traeger_map_t *traeger_map_copy(const traeger_map_t *self);

    void traeger_map_free(traeger_map_t *self);

    void traeger_map_print(const traeger_map_t *self);

    void traeger_map_to_string(const traeger_map_t *self,
                               traeger_string_t **string);

    bool traeger_map_equal(const traeger_map_t *self,
                           const traeger_map_t *other);

    void traeger_map_set_value(traeger_map_t *self,
                               const char *key_data,
                               size_t key_size,
                               const traeger_value_t *value);

    void traeger_map_set_null(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size);

    void traeger_map_set_bool(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              traeger_bool_t value);

    void traeger_map_set_int(traeger_map_t *self,
                             const char *key_data,
                             size_t key_size,
                             traeger_int_t value);

    void traeger_map_set_uint(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              traeger_uint_t value);

    void traeger_map_set_float(traeger_map_t *self,
                               const char *key_data,
                               size_t key_size,
                               traeger_float_t value);

    void traeger_map_set_string(traeger_map_t *self,
                                const char *key_data,
                                size_t key_size,
                                const char *string_data,
                                size_t string_size);

    void traeger_map_set_list(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              const traeger_list_t *list);

    void traeger_map_set_map(traeger_map_t *self,
                             const char *key_data,
                             size_t key_size,
                             const traeger_map_t *map);

    void traeger_map_erase(traeger_map_t *self,
                           const char *key_data,
                           size_t key_size);

    bool traeger_map_contains(const traeger_map_t *self,
                              const char *key_data,
                              size_t key_size);

    bool traeger_map_find(const traeger_map_t *self,
                          const char *key_data,
                          size_t key_size,
                          traeger_value_t **value);

    bool traeger_map_empty(const traeger_map_t *self);

    size_t traeger_map_size(const traeger_map_t *self);

    // Map::Iterator

    traeger_map_iterator_t *traeger_map_iterator_new(const traeger_map_t *self);

    bool traeger_map_iterator_has_next(const traeger_map_iterator_t *self);

    bool traeger_map_iterator_next(traeger_map_iterator_t *self,
                                   traeger_string_t **key,
                                   traeger_value_t **value);

    void traeger_map_iterator_free(traeger_map_iterator_t *self);

    // Value

    traeger_value_t *traeger_value_new();

    traeger_value_t *traeger_value_copy(const traeger_value_t *self);

    void traeger_value_free(traeger_value_t *self);

    void traeger_value_print(const traeger_value_t *self);

    void traeger_value_to_string(const traeger_value_t *self,
                                 traeger_string_t **string);

    bool traeger_value_equal(const traeger_value_t *self,
                             const traeger_value_t *other);

    traeger_value_type_t traeger_value_get_type(const traeger_value_t *self);

    const char *traeger_value_get_type_name(const traeger_value_t *self);

    const char *traeger_value_type_get_name(traeger_value_type_t type);

    void traeger_value_set_null(traeger_value_t *self);

    void traeger_value_set_bool(traeger_value_t *self,
                                traeger_bool_t value);

    void traeger_value_set_int(traeger_value_t *self,
                               traeger_int_t value);

    void traeger_value_set_uint(traeger_value_t *self,
                                traeger_uint_t value);

    void traeger_value_set_float(traeger_value_t *self,
                                 traeger_float_t value);

    void traeger_value_set_string(traeger_value_t *self,
                                  const char *string_data,
                                  size_t string_size);

    void traeger_value_set_list(traeger_value_t *self,
                                const traeger_list_t *list);

    void traeger_value_set_map(traeger_value_t *self,
                               const traeger_map_t *map);

    void traeger_value_set_value(traeger_value_t *self,
                                 const traeger_value_t *value);

    bool traeger_value_get_null(const traeger_value_t *self);

    bool traeger_value_get_bool(const traeger_value_t *self,
                                traeger_bool_t *value);

    bool traeger_value_get_int(const traeger_value_t *self,
                               traeger_int_t *value);

    bool traeger_value_get_uint(const traeger_value_t *self,
                                traeger_uint_t *value);

    bool traeger_value_get_float(const traeger_value_t *self,
                                 traeger_float_t *value);

    bool traeger_value_get_string(const traeger_value_t *self,
                                  traeger_string_t **string);

    bool traeger_value_get_list(const traeger_value_t *self,
                                traeger_list_t **list);

    bool traeger_value_get_map(const traeger_value_t *self,
                               traeger_map_t **map);

#ifdef __cplusplus
}
#endif
