// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/value/value.h>

typedef struct traeger_function_t traeger_function_t;

typedef struct traeger_work_t traeger_work_t;

typedef struct traeger_scheduler_t traeger_scheduler_t;

typedef struct traeger_promise_t traeger_promise_t;

typedef struct traeger_mailbox_t traeger_mailbox_t;

typedef struct traeger_actor_t traeger_actor_t;

typedef struct traeger_queue_t traeger_queue_t;

typedef struct traeger_group_t traeger_group_t;

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *traeger_closure_t;

    typedef void (*traeger_closure_free_t)(traeger_closure_t);

    // Result

    traeger_result_t *traeger_result_new();

    void traeger_result_free(traeger_result_t *self);

    void traeger_result_set_value(traeger_result_t *self,
                                  const traeger_value_t *value);

    void traeger_result_set_null(traeger_result_t *self, void *);

    void traeger_result_set_bool(traeger_result_t *self,
                                 traeger_bool_t value);

    void traeger_result_set_int(traeger_result_t *self,
                                traeger_int_t value);

    void traeger_result_set_uint(traeger_result_t *self,
                                 traeger_uint_t value);

    void traeger_result_set_float(traeger_result_t *self,
                                  traeger_float_t value);

    void traeger_result_set_string(traeger_result_t *self,
                                   const char *string_data,
                                   size_t string_size);

    void traeger_result_set_list(traeger_result_t *self,
                                 const traeger_list_t *list);

    void traeger_result_set_map(traeger_result_t *self,
                                const traeger_map_t *map);

    void traeger_result_set_error(traeger_result_t *self,
                                  const char *error_data,
                                  size_t error_size);

    void traeger_result_set_result(traeger_result_t *self,
                                   const traeger_result_t *result);

    bool traeger_result_get_value_or_error(const traeger_result_t *self,
                                           traeger_value_t **value,
                                           traeger_string_t **error);

    // Function

    typedef void (*traeger_function_callback_t)(traeger_list_t *arguments,
                                                traeger_closure_t closure,
                                                traeger_result_t *result);

    traeger_function_t *traeger_function_new(traeger_function_callback_t function_callback,
                                             traeger_closure_t closure,
                                             traeger_closure_free_t closure_free);

    void traeger_function_free(traeger_function_t *self);

    bool traeger_function_call(const traeger_function_t *self,
                               const traeger_list_t *arguments,
                               traeger_result_t **result);

    // Scheduler

    traeger_scheduler_t *traeger_scheduler_new(unsigned int threads_count);

    void traeger_scheduler_free(traeger_scheduler_t *self);

    size_t traeger_scheduler_count(const traeger_scheduler_t *self);

    typedef void (*traeger_work_callback_t)(traeger_closure_t closure);

    void traeger_scheduler_schedule(const traeger_scheduler_t *self,
                                    traeger_work_callback_t work_callback,
                                    traeger_closure_t closure,
                                    traeger_closure_free_t closure_free);

    void traeger_scheduler_schedule_delayed(const traeger_scheduler_t *self,
                                            traeger_float_t delay,
                                            traeger_work_callback_t work_callback,
                                            traeger_closure_t closure,
                                            traeger_closure_free_t closure_free);

    // Promise

    traeger_promise_t *traeger_promise_new(traeger_scheduler_t *scheduler);

    void traeger_promise_free(traeger_promise_t *self);

    bool traeger_promise_set_result(const traeger_promise_t *self,
                                    const traeger_result_t *result);

    void traeger_promise_get_result(const traeger_promise_t *self,
                                    traeger_result_t **result);

    void traeger_promise_set_promise(traeger_promise_t *self,
                                     const traeger_promise_t *promise);

    typedef void (*traeger_promise_result_callback_t)(traeger_value_t *value,
                                                      traeger_closure_t closure,
                                                      traeger_result_t *result);

    typedef void (*traeger_promise_promise_callback_t)(traeger_value_t *value,
                                                       traeger_closure_t closure,
                                                       traeger_promise_t *promise);

    traeger_promise_t *traeger_promise_then_result(const traeger_promise_t *self,
                                                   traeger_promise_result_callback_t result_callback,
                                                   traeger_closure_t closure,
                                                   traeger_closure_free_t closure_free);

    traeger_promise_t *traeger_promise_then_promise(const traeger_promise_t *self,
                                                    traeger_promise_promise_callback_t result_callback,
                                                    traeger_closure_t closure,
                                                    traeger_closure_free_t closure_free);

    typedef void (*traeger_promise_error_callback_t)(traeger_string_t *error,
                                                     traeger_closure_t closure);

    void traeger_promise_fail(const traeger_promise_t *self,
                              traeger_promise_error_callback_t error_callback,
                              traeger_closure_t closure,
                              traeger_closure_free_t closure_free);

    // Mailbox

    void traeger_mailbox_free(traeger_mailbox_t *self);

    traeger_promise_t *traeger_mailbox_send(const traeger_mailbox_t *self,
                                            const traeger_scheduler_t *scheduler,
                                            const char *name_data,
                                            size_t name_size,
                                            const traeger_list_t *arguments);

    // Actor

    traeger_actor_t *traeger_actor_new();

    void traeger_actor_free(traeger_actor_t *self);

    traeger_mailbox_t *traeger_actor_get_mailbox(const traeger_actor_t *self);

    void traeger_actor_define_reader(const traeger_actor_t *self,
                                     const char *name_data,
                                     size_t name_size,
                                     traeger_function_callback_t function_callback,
                                     traeger_closure_t closure,
                                     traeger_closure_free_t closure_free);

    void traeger_actor_define_writer(const traeger_actor_t *self,
                                     const char *name_data,
                                     size_t name_size,
                                     traeger_function_callback_t function_callback,
                                     traeger_closure_t closure,
                                     traeger_closure_free_t closure_free);

    // Queue

    traeger_queue_t *traeger_queue_new();

    void traeger_queue_free(traeger_queue_t *self);

    bool traeger_queue_closed(const traeger_queue_t *self);

    size_t traeger_queue_count(const traeger_queue_t *self);

    bool traeger_queue_push_null(const traeger_queue_t *self,
                                 void *);

    bool traeger_queue_push_bool(const traeger_queue_t *self,
                                 traeger_bool_t value);

    bool traeger_queue_push_int(const traeger_queue_t *self,
                                traeger_int_t value);

    bool traeger_queue_push_uint(const traeger_queue_t *self,
                                 traeger_uint_t value);

    bool traeger_queue_push_float(const traeger_queue_t *self,
                                  traeger_float_t value);

    bool traeger_queue_push_string(const traeger_queue_t *self,
                                   const char *string_data,
                                   size_t string_size);

    bool traeger_queue_push_list(const traeger_queue_t *self,
                                 const traeger_list_t *list);

    bool traeger_queue_push_map(const traeger_queue_t *self,
                                const traeger_map_t *map);

    bool traeger_queue_push_value(const traeger_queue_t *self,
                                  const traeger_value_t *value);

    bool traeger_queue_pop_value(const traeger_queue_t *self,
                                 traeger_value_t **value);

    bool traeger_queue_pop_into_list(const traeger_queue_t *self,
                                     traeger_list_t *list);

    void traeger_queue_close(const traeger_queue_t *self);

#ifdef __cplusplus
}
#endif
