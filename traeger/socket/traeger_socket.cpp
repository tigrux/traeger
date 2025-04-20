// SPDX-License-Identifier: BSL-1.0

#include "traeger/value/traeger_value.hpp"
#include "traeger/format/traeger_format.hpp"
#include "traeger/actor/traeger_actor.hpp"
#include "traeger/socket/traeger_socket.hpp"

namespace
{
    using namespace traeger;

    Subscriber::Callback make_subscriber_callback(traeger_subscriber_callback_t callback,
                                                  traeger_closure_t closure,
                                                  traeger_closure_free_t closure_free) noexcept
    {
        return [closure = std::shared_ptr<void>{closure, closure_free},
                callback](String string, Value value) -> void
        {
            const traeger_string_t topic{std::move(string)};
            auto *event = new traeger_value_t{std::move(value)};
            callback(&topic, event, closure.get());
        };
    }

    std::vector<String> list_to_strings(const List &list)
    {
        std::vector<String> result;
        result.reserve(list.size());
        for (const auto &value : list)
        {
            if (const auto *s = value.get_string(); s)
            {
                result.push_back(*s);
            }
        }
        return result;
    }
}

extern "C"
{
    using namespace traeger;

    // Context

    traeger_context_t *traeger_context_new()
    {
        return new traeger_context_t{Context{}};
    }

    void traeger_context_free(traeger_context_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    // Replier

    bool traeger_replier_new(const traeger_context_t *self,
                             const char *address,
                             traeger_replier_t **result,
                             traeger_string_t **error)
    {
        if (self != nullptr &&
            address != nullptr &&
            result != nullptr)
        {
            auto [replier, socket_error] = cast(self).replier(address);
            if (replier)
            {
                *result = new traeger_replier_t{std::move(replier).value()};
                return true;
            }
            else if (error)
            {
                *error = new traeger_string_t{std::move(socket_error)};
            }
        }
        return false;
    }

    void traeger_replier_free(traeger_replier_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    traeger_promise_t *traeger_replier_reply(const traeger_replier_t *self,
                                             const traeger_scheduler_t *scheduler,
                                             const traeger_mailbox_t *mailbox)
    {
        if (self != nullptr &&
            scheduler != nullptr &&
            mailbox != nullptr)
        {
            return new traeger_promise_t{cast(self).reply(cast(scheduler), cast(mailbox))};
        }
        return nullptr;
    }

    // Requester

    bool traeger_requester_new(const traeger_context_t *self,
                               const char *address,
                               const traeger_format_t *format,
                               traeger_requester_t **result,
                               traeger_string_t **error)
    {
        if (self != nullptr &&
            address != nullptr &&
            format != nullptr &&
            result != nullptr)
        {
            auto [requester, socket_error] = cast(self).requester(address, cast(format));
            if (requester)
            {
                *result = new traeger_requester_t{std::move(requester).value()};
                return true;
            }
            else if (error)
            {
                *error = new traeger_string_t{std::move(socket_error)};
            }
        }
        return false;
    }

    void traeger_requester_free(traeger_requester_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    traeger_mailbox_t *traeger_requester_get_mailbox(const traeger_requester_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_mailbox_t{cast(self).mailbox()};
        }
        return nullptr;
    }

    // Publisher

    bool traeger_publisher_new(const traeger_context_t *self,
                               const char *address,
                               const traeger_format_t *format,
                               traeger_publisher_t **result,
                               traeger_string_t **error)
    {
        if (self != nullptr &&
            address != nullptr &&
            format != nullptr &&
            address != nullptr &&
            result != nullptr)
        {
            // return new traeger_publisher_t{cast(self).publisher(cast(format))};
            auto [publisher, socket_error] = cast(self).publisher(address, cast(format));
            if (publisher)
            {
                *result = new traeger_publisher_t{std::move(publisher).value()};
                return true;
            }
            else if (error)
            {
                *error = new traeger_string_t{std::move(socket_error)};
            }
        }
        return false;
    }

    void traeger_publisher_free(traeger_publisher_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    traeger_promise_t *traeger_publisher_publish(const traeger_publisher_t *self,
                                                 const traeger_scheduler_t *scheduler,
                                                 const char *topic_data,
                                                 size_t topic_size,
                                                 const traeger_value_t *value)
    {
        if (self != nullptr &&
            scheduler != nullptr &&
            topic_data != nullptr &&
            value != nullptr)
        {
            return new traeger_promise_t{cast(self).publish(cast(scheduler), String(topic_data, topic_size), cast(value))};
        }
        return nullptr;
    }

    // Subscriber

    bool traeger_subscriber_new(const traeger_context_t *self,
                                const char *address,
                                const traeger_list_t *topics,
                                traeger_subscriber_t **result,
                                traeger_string_t **error)
    {
        if (self != nullptr &&
            address != nullptr &&
            topics != nullptr &&
            result != nullptr)
        {
            auto [subscriber, socket_error] = cast(self).subscriber(address, list_to_strings(cast(topics)));
            if (subscriber)
            {
                *result = new traeger_subscriber_t{std::move(subscriber).value()};
                return true;
            }
            else if (error != nullptr)
            {
                *error = new traeger_string_t{std::move(socket_error)};
            }
        }
        return false;
    }

    void traeger_subscriber_free(traeger_subscriber_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    traeger_promise_t *traeger_subscriber_listen(const traeger_subscriber_t *self,
                                                 const traeger_scheduler_t *scheduler,
                                                 traeger_subscriber_callback_t callback,
                                                 traeger_closure_t closure,
                                                 traeger_closure_free_t closure_free)
    {
        if (self != nullptr &&
            scheduler != nullptr &&
            callback != nullptr &&
            closure != nullptr &&
            closure_free != nullptr)
        {
            return new traeger_promise_t{cast(self).listen(
                cast(scheduler),
                make_subscriber_callback(callback, closure, closure_free))};
        }
        return nullptr;
    }
}
