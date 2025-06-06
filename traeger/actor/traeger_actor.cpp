// SPDX-License-Identifier: BSL-1.0

#include "traeger/actor/actor.h"
#include "traeger/value/traeger_value.hpp"
#include "traeger/actor/traeger_actor.hpp"

namespace
{
    using namespace traeger;

    Work make_work(traeger_work_callback_t work_callback,
                   const traeger_closure_t closure,
                   const traeger_closure_free_t closure_free) noexcept
    {
        return [closure = std::shared_ptr<void>{closure, closure_free},
                work_callback]() -> void
        {
            work_callback(closure.get());
        };
    }

    Function make_function(traeger_function_callback_t function_callback,
                           const traeger_closure_t closure,
                           const traeger_closure_free_t closure_free) noexcept
    {
        return
            [closure = std::shared_ptr<void>{closure, closure_free},
             function_callback](List list) -> Result
        {
            traeger_result_t result;
            const auto *arguments = new traeger_list_t{std::move(list)};
            function_callback(arguments, closure.get(), &result);
            return std::move(result);
        };
    }

    Promise::ResultCallback make_promise_result_callback(traeger_promise_result_callback_t result_callback,
                                                         const traeger_closure_t closure,
                                                         const traeger_closure_free_t closure_free)
    {
        return [closure = std::shared_ptr<void>{closure, closure_free},
                result_callback](const Value &argument) -> Result
        {
            traeger_result_t result;
            const auto *value = new traeger_value_t{argument};
            result_callback(value, closure.get(), &result);
            return std::move(result);
        };
    }

    Promise::PromiseCallback make_promise_promise_callback(traeger_promise_promise_callback_t promise_callback,
                                                           const traeger_closure_t closure,
                                                           const traeger_closure_free_t closure_free,
                                                           const Scheduler &scheduler)
    {
        return [closure = std::shared_ptr<void>{closure, closure_free},
                promise_callback, scheduler](const Value &argument) -> Promise
        {
            traeger_promise_t promise{Promise{scheduler}};
            const auto *value = new traeger_value_t{argument};
            promise_callback(value, closure.get(), &promise);
            return std::move(promise);
        };
    }

    Promise::ErrorCallback make_promise_error_callback(traeger_promise_error_callback_t error_callback,
                                                       const traeger_closure_t closure,
                                                       const traeger_closure_free_t closure_free)
    {
        return [closure = std::shared_ptr<void>{closure, closure_free},
                error_callback](const String &argument) -> void
        {
            const traeger_string_t error{argument};
            error_callback(&error, closure.get());
        };
    }
}

extern "C"
{
    using namespace traeger;

    // Result

    traeger_result_t *traeger_result_new()
    {
        return new traeger_result_t{};
    }

    traeger_result_t *traeger_result_copy(const traeger_result_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_result_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_result_free(traeger_result_t *self)
    {

        delete self;
    }

    void traeger_result_set_value(traeger_result_t *self,
                                  const traeger_value_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            cast(self) = Result{*value};
        }
    }

    void traeger_result_set_null(traeger_result_t *self, void *)
    {
        if (self != nullptr)
        {
            cast(self) = Result{Value{nullptr}};
        }
    }

    void traeger_result_set_bool(traeger_result_t *self,
                                 const traeger_bool_t value)

    {
        if (self != nullptr)
        {
            cast(self) = Result{Value{value}};
        }
    }

    void traeger_result_set_int(traeger_result_t *self,
                                const traeger_int_t value)
    {
        if (self != nullptr)
        {
            cast(self) = Result{Value{value}};
        }
    }

    void traeger_result_set_uint(traeger_result_t *self,
                                 const traeger_uint_t value)
    {
        if (self != nullptr)
        {
            cast(self) = Result{Value{value}};
        }
    }

    void traeger_result_set_float(traeger_result_t *self,
                                  const traeger_float_t value)
    {
        if (self != nullptr)
        {
            cast(self) = Result{Value{value}};
        }
    }

    void traeger_result_set_string(traeger_result_t *self,
                                   const char *string_data,
                                   const size_t string_size)
    {
        if (self != nullptr &&
            string_data != nullptr)
        {
            cast(self) = Result{Value{String(string_data, string_size)}};
        }
    }

    void traeger_result_set_list(traeger_result_t *self,
                                 const traeger_list_t *list)
    {
        if (self != nullptr &&
            list != nullptr)
        {
            cast(self) = Result{Value{cast(list)}};
        }
    }

    void traeger_result_set_map(traeger_result_t *self,
                                const traeger_map_t *map)
    {
        if (self != nullptr &&
            map != nullptr)
        {
            cast(self) = Result{Value{cast(map)}};
        }
    }

    void traeger_result_set_error(traeger_result_t *self,
                                  const char *error_data,
                                  const size_t error_size)
    {
        if (self != nullptr &&
            error_data != nullptr)
        {
            cast(self) = Result{Error{String(error_data, error_size)}};
        }
    }

    void traeger_result_set_result(traeger_result_t *self,
                                   const traeger_result_t *result)
    {
        if (self != nullptr &&
            result != nullptr)
        {
            *self = *result;
        }
    }

    traeger_result_type_t
    traeger_result_get_value_or_error(const traeger_result_t *self,
                                      traeger_value_t **value,
                                      traeger_string_t **error)
    {
        if (self != nullptr)
        {
            switch (cast(self).type())
            {
            case Result::Type::Undefined:
                return TRAEGER_RESULT_TYPE_UNDEFINED;
            case Result::Type::Value:
                *value = new traeger_value_t{*cast(self).value()};
                return TRAEGER_RESULT_TYPE_VALUE;
            case Result::Type::Error:
                *error = new traeger_string_t{*cast(self).error()};
                return TRAEGER_RESULT_TYPE_ERROR;
            }
        }
        return TRAEGER_RESULT_TYPE_UNDEFINED;
    }

    // Function

    traeger_function_t *traeger_function_new(const traeger_function_callback_t function_callback,
                                             const traeger_closure_t closure,
                                             const traeger_closure_free_t closure_free)
    {
        if (function_callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            return new traeger_function_t{make_function(function_callback, closure, closure_free)};
        }
        return nullptr;
    }

    traeger_function_t *traeger_function_copy(const traeger_function_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_function_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_function_free(traeger_function_t *self)
    {

        delete self;
    }

    traeger_result_type_t
    traeger_function_call(const traeger_function_t *self,
                          const traeger_list_t *arguments,
                          traeger_result_t **result)
    {
        if (self != nullptr &&
            arguments != nullptr &&
            result != nullptr)
        {
            *result = new traeger_result_t{cast(self)(cast(arguments))};
            switch ((*result)->type())
            {
            case Result::Type::Undefined:
                return TRAEGER_RESULT_TYPE_UNDEFINED;
            case Result::Type::Value:
                return TRAEGER_RESULT_TYPE_VALUE;
            case Result::Type::Error:
                return TRAEGER_RESULT_TYPE_ERROR;
            }
        }
        return TRAEGER_RESULT_TYPE_UNDEFINED;
    }

    // Scheduler

    traeger_scheduler_t *traeger_scheduler_new(const unsigned int threads_count)
    {
        return new traeger_scheduler_t{Scheduler{Threads{threads_count}}};
    }

    traeger_scheduler_t *traeger_scheduler_copy(const traeger_scheduler_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_scheduler_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_scheduler_free(traeger_scheduler_t *self)
    {

        delete self;
    }

    size_t traeger_scheduler_count(const traeger_scheduler_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).count();
        }
        return 0;
    }

    void traeger_scheduler_schedule(const traeger_scheduler_t *self,
                                    const traeger_work_callback_t work_callback,
                                    const traeger_closure_t closure,
                                    const traeger_closure_free_t closure_free)
    {
        if (self != nullptr &&
            work_callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            cast(self).schedule(make_work(work_callback, closure, closure_free));
        }
    }

    void traeger_scheduler_schedule_delayed(const traeger_scheduler_t *self,
                                            const traeger_float_t delay,
                                            const traeger_work_callback_t work_callback,
                                            const traeger_closure_t closure,
                                            const traeger_closure_free_t closure_free)
    {
        if (self != nullptr &&
            work_callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            cast(self).schedule_delayed(to_microseconds(delay),
                                        make_work(work_callback, closure, closure_free));
        }
    }

    // Promise

    traeger_promise_t *traeger_promise_new(const traeger_scheduler_t *scheduler)
    {
        if (scheduler != nullptr)
        {
            return new traeger_promise_t{Promise{*scheduler}};
        }
        return nullptr;
    }

    traeger_promise_t *traeger_promise_copy(const traeger_promise_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_promise_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_promise_free(traeger_promise_t *self)
    {

        delete self;
    }

    void traeger_promise_set_promise(traeger_promise_t *self,
                                     const traeger_promise_t *promise)
    {
        if (self != nullptr &&
            promise != nullptr)
        {
            cast(self) = cast(promise);
        }
    }

    void traeger_promise_get_result(const traeger_promise_t *self,
                                    traeger_result_t **result)
    {
        if (self != nullptr &&
            result != nullptr)
        {
            *result = new traeger_result_t{cast(self).result()};
        }
    }

    bool traeger_promise_set_result(const traeger_promise_t *self,
                                    const traeger_result_t *result)
    {
        if (self != nullptr &&
            result != nullptr)
        {
            return cast(self).set_result(cast(result));
        }
        return false;
    }

    void traeger_promise_set_result_from_promise(const traeger_promise_t *self,
                                                 const traeger_promise_t *promise)
    {
        if (self != nullptr &&
            promise != nullptr)
        {
            cast(self).set_result_from_promise(cast(promise));
        }
    }

    traeger_promise_t *traeger_promise_then_result(const traeger_promise_t *self,
                                                   const traeger_promise_result_callback_t result_callback,
                                                   const traeger_closure_t closure,
                                                   const traeger_closure_free_t closure_free)
    {
        if (self != nullptr &&
            result_callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            return new traeger_promise_t{cast(self).then(make_promise_result_callback(result_callback,
                                                                                      closure,
                                                                                      closure_free))};
        }
        return nullptr;
    }

    traeger_promise_t *traeger_promise_then_promise(const traeger_promise_t *self,
                                                    const traeger_promise_promise_callback_t promise_callback,
                                                    const traeger_closure_t closure,
                                                    const traeger_closure_free_t closure_free)
    {
        if (self != nullptr &&
            promise_callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            return new traeger_promise_t{cast(self).then(make_promise_promise_callback(promise_callback,
                                                                                       closure,
                                                                                       closure_free,
                                                                                       cast(self).scheduler()))};
        }
        return nullptr;
    }

    void traeger_promise_fail(const traeger_promise_t *self,
                              const traeger_promise_error_callback_t error_callback,
                              const traeger_closure_t closure,
                              const traeger_closure_free_t closure_free)
    {
        if (self != nullptr &&
            error_callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            cast(self).fail(make_promise_error_callback(error_callback,
                                                        closure,
                                                        closure_free));
        }
    }

    // Mailbox

    traeger_mailbox_t *traeger_mailbox_copy(const traeger_mailbox_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_mailbox_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_mailbox_free(traeger_mailbox_t *self)
    {

        delete self;
    }

    traeger_promise_t *traeger_mailbox_send(const traeger_mailbox_t *self,
                                            const traeger_scheduler_t *scheduler,
                                            const char *name_data,
                                            const size_t name_size,
                                            const traeger_list_t *arguments)
    {
        if (self != nullptr &&
            scheduler != nullptr &&
            name_data != nullptr &&
            arguments != nullptr)
        {
            return new traeger_promise_t{cast(self).send(cast(scheduler),
                                                         String(name_data, name_size),
                                                         cast(arguments))};
        }
        return nullptr;
    }

    // Actor

    traeger_actor_t *traeger_actor_new()
    {
        return new traeger_actor_t{StatelessActor{}};
    }

    void traeger_actor_free(traeger_actor_t *self)
    {

        delete self;
    }

    traeger_mailbox_t *traeger_actor_get_mailbox(const traeger_actor_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_mailbox_t{cast(self).mailbox()};
        }
        return nullptr;
    }

    traeger_mailbox_interface_t *traeger_actor_get_mailbox_interface(const traeger_actor_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).mailbox_interface().release();
        }
        return nullptr;
    }

    void traeger_actor_define_reader(const traeger_actor_t *self,
                                     const char *name_data,
                                     const size_t name_size,
                                     const traeger_function_t *function)
    {
        if (self != nullptr &&
            name_data != nullptr &&
            function != nullptr)
        {
            cast(self).define_reader(
                String(name_data, name_size),
                cast(function));
        }
    }

    void traeger_actor_define_writer(const traeger_actor_t *self,
                                     const char *name_data,
                                     const size_t name_size,
                                     const traeger_function_t *function)
    {
        if (self != nullptr &&
            name_data != nullptr &&
            function != nullptr)
        {
            cast(self).define_writer(
                String(name_data, name_size),
                cast(function));
        }
    }

    // Queue

    traeger_queue_t *traeger_queue_new()
    {
        return new traeger_queue_t{};
    }

    traeger_queue_t *traeger_queue_copy(const traeger_queue_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_queue_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_queue_free(traeger_queue_t *self)
    {
        delete self;
    }

    bool traeger_queue_closed(const traeger_queue_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).closed();
        }
        return false;
    }

    size_t traeger_queue_count(const traeger_queue_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).count();
        }
        return 0;
    }

    bool traeger_queue_push_value(const traeger_queue_t *self,
                                  const traeger_value_t *value)
    {
        if (self != nullptr)
        {
            return cast(self).push(*value);
        }
        return false;
    }

    bool traeger_queue_push_null(const traeger_queue_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).push(nullptr);
        }
        return false;
    }

    bool traeger_queue_push_bool(const traeger_queue_t *self,
                                 const traeger_bool_t value)
    {
        if (self != nullptr)
        {
            return cast(self).push(value);
        }
        return false;
    }

    bool traeger_queue_push_int(const traeger_queue_t *self,
                                const traeger_int_t value)
    {
        if (self != nullptr)
        {
            return cast(self).push(value);
        }
        return false;
    }

    bool traeger_queue_push_uint(const traeger_queue_t *self,
                                 const traeger_uint_t value)
    {
        if (self != nullptr)
        {
            return cast(self).push(value);
        }
        return false;
    }

    bool traeger_queue_push_float(const traeger_queue_t *self,
                                  const traeger_float_t value)
    {
        if (self != nullptr)
        {
            return cast(self).push(value);
        }
        return false;
    }

    bool traeger_queue_push_string(const traeger_queue_t *self,
                                   const char *string_data,
                                   const size_t string_size)
    {
        if (self != nullptr)
        {
            return cast(self).push(String(string_data, string_size));
        }
        return false;
    }

    bool traeger_queue_push_list(const traeger_queue_t *self,
                                 const traeger_list_t *list)
    {
        if (self != nullptr)
        {
            return cast(self).push(cast(list));
        }
        return false;
    }

    bool traeger_queue_push_map(const traeger_queue_t *self,
                                const traeger_map_t *map)
    {
        if (self != nullptr)
        {
            return cast(self).push(cast(map));
        }
        return false;
    }

    bool traeger_queue_pop_value(const traeger_queue_t *self,
                                 traeger_value_t **value)
    {
        if (self != nullptr)
        {
            if (auto optional = cast(self).pop(); optional)
            {
                *value = new traeger_value_t{std::move(optional).value()};
                return true;
            }
        }
        return false;
    }

    bool traeger_queue_pop_into_list(const traeger_queue_t *self,
                                     traeger_list_t *list)
    {
        if (self != nullptr &&
            list != nullptr)
        {
            return cast(self).pop(cast(list));
        }
        return false;
    }

    void traeger_queue_close(const traeger_queue_t *self)
    {
        if (self != nullptr)
        {
            cast(self).close();
        }
    }
}
