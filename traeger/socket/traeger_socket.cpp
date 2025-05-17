// SPDX-License-Identifier: BSL-1.0

#include "traeger/value/traeger_value.hpp"
#include "traeger/format/traeger_format.hpp"
#include "traeger/actor/traeger_actor.hpp"
#include "traeger/socket/traeger_socket.hpp"

namespace
{
    using namespace traeger;

    Subscriber::Callback make_subscriber_callback(const Function &function) noexcept
    {
        return [function](String topic, Value value) -> void
        {
            function(make_list(topic, value));
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

    traeger_context_t *traeger_context_copy(const traeger_context_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_context_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_context_free(traeger_context_t *self)
    {

        delete self;
    }

    // Replier

    bool traeger_replier_new(const traeger_context_t *self,
                             const char *address_data,
                             const size_t address_size,
                             traeger_replier_t **result,
                             traeger_string_t **error)
    {
        if (self != nullptr &&
            address_data != nullptr &&
            result != nullptr)
        {

            if (auto [replier, socket_error] = cast(self).replier(String(address_data, address_size));
                replier)
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

    traeger_replier_t *traeger_replier_copy(const traeger_replier_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_replier_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_replier_free(traeger_replier_t *self)
    {

        delete self;
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
                               const char *address_data,
                               const size_t address_size,
                               const traeger_format_t *format,
                               traeger_requester_t **result,
                               traeger_string_t **error)
    {
        if (self != nullptr &&
            address_data != nullptr &&
            format != nullptr &&
            result != nullptr)
        {
            if (auto [requester, socket_error] = cast(self).requester(String(address_data, address_size), cast(format));
                requester)
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

    traeger_requester_t *traeger_requester_copy(const traeger_requester_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_requester_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_requester_free(traeger_requester_t *self)
    {

        delete self;
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
                               const char *address_data,
                               const size_t address_size,
                               const traeger_format_t *format,
                               traeger_publisher_t **result,
                               traeger_string_t **error)
    {
        if (self != nullptr &&
            address_data != nullptr &&
            format != nullptr &&
            result != nullptr)
        {
            // return new traeger_publisher_t{cast(self).publisher(cast(format))};
            if (auto [publisher, socket_error] = cast(self).publisher(String(address_data, address_size), cast(format));
                publisher)
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

    traeger_publisher_t *traeger_publisher_copy(const traeger_publisher_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_publisher_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_publisher_free(traeger_publisher_t *self)
    {

        delete self;
    }

    traeger_promise_t *traeger_publisher_publish(const traeger_publisher_t *self,
                                                 const traeger_scheduler_t *scheduler,
                                                 const char *topic_data,
                                                 const size_t topic_size,
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
                                const char *address_data,
                                const size_t address_size,
                                const traeger_list_t *topics,
                                traeger_subscriber_t **result,
                                traeger_string_t **error)
    {
        if (self != nullptr &&
            address_data != nullptr &&
            topics != nullptr &&
            result != nullptr)
        {
            if (auto [subscriber, socket_error] = cast(self).subscriber(String(address_data, address_size), list_to_strings(cast(topics)));
                subscriber)
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

    traeger_subscriber_t *traeger_subscriber_copy(const traeger_subscriber_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_subscriber_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_subscriber_free(traeger_subscriber_t *self)
    {

        delete self;
    }

    traeger_promise_t *traeger_subscriber_listen(const traeger_subscriber_t *self,
                                                 const traeger_scheduler_t *scheduler,
                                                 const traeger_function_t *function)
    {
        if (self != nullptr &&
            scheduler != nullptr &&
            function != nullptr)
        {
            return new traeger_promise_t{cast(self).listen(
                cast(scheduler),
                make_subscriber_callback(cast(function)))};
        }
        return nullptr;
    }
}
