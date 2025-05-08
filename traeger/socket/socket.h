// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/format/format.h>
#include <traeger/actor/actor.h>

typedef struct traeger_context_t traeger_context_t;

typedef struct traeger_replier_t traeger_replier_t;

typedef struct traeger_requester_t traeger_requester_t;

typedef struct traeger_publisher_t traeger_publisher_t;

typedef struct traeger_subscriber_t traeger_subscriber_t;

#ifdef __cplusplus
extern "C"
{
#endif

    // Context

    traeger_context_t *traeger_context_new();

    traeger_context_t *traeger_context_copy(const traeger_context_t *self);

    void traeger_context_free(traeger_context_t *self);

    // Replier

    bool traeger_replier_new(const traeger_context_t *self,
                             const char *address_data,
                             size_t address_size,
                             traeger_replier_t **result,
                             traeger_string_t **error);

    traeger_replier_t *traeger_replier_copy(const traeger_replier_t *self);

    void traeger_replier_free(traeger_replier_t *self);

    traeger_promise_t *traeger_replier_reply(const traeger_replier_t *self,
                                             const traeger_scheduler_t *scheduler,
                                             const traeger_mailbox_t *mailbox);

    // Requester

    bool traeger_requester_new(const traeger_context_t *self,
                               const char *address_data,
                               size_t address_size,
                               const traeger_format_t *format,
                               traeger_requester_t **result,
                               traeger_string_t **error);

    traeger_requester_t *traeger_requester_copy(const traeger_requester_t *self);

    void traeger_requester_free(traeger_requester_t *self);

    traeger_mailbox_t *traeger_requester_get_mailbox(const traeger_requester_t *self);

    // Publisher

    bool traeger_publisher_new(const traeger_context_t *self,
                               const char *address_data,
                               size_t address_size,
                               const traeger_format_t *format,
                               traeger_publisher_t **result,
                               traeger_string_t **error);

    traeger_publisher_t *traeger_publisher_copy(const traeger_publisher_t *self);

    void traeger_publisher_free(traeger_publisher_t *self);

    traeger_promise_t *traeger_publisher_publish(const traeger_publisher_t *self,
                                                 const traeger_scheduler_t *scheduler,
                                                 const char *topic_data,
                                                 size_t topic_size,
                                                 const traeger_value_t *value);

    // Subscriber

    bool traeger_subscriber_new(const traeger_context_t *self,
                                const char *address_data,
                                size_t address_size,
                                const traeger_list_t *topics,
                                traeger_subscriber_t **result,
                                traeger_string_t **error);

    traeger_subscriber_t *traeger_subscriber_copy(const traeger_subscriber_t *self);

    void traeger_subscriber_free(traeger_subscriber_t *self);

    typedef void (*traeger_subscriber_callback_t)(const traeger_string_t *topic,
                                                  traeger_value_t *value,
                                                  traeger_closure_t closure);

    traeger_promise_t *traeger_subscriber_listen(const traeger_subscriber_t *self,
                                                 const traeger_scheduler_t *scheduler,
                                                 const traeger_function_t *function);

#ifdef __cplusplus
}
#endif
