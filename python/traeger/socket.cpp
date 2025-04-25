// SPDX-License-Identifier: BSL-1.0

#include <nanobind/stl/string.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>

#include <traeger/value/Variant.hpp>
#include <traeger/value/Value.hpp>
#include <traeger/actor/Promise.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Replier.hpp>
#include <traeger/socket/Requester.hpp>
#include <traeger/socket/Publisher.hpp>
#include <traeger/socket/Subscriber.hpp>

#include "traeger.hpp"

namespace
{
    namespace nb = nanobind;
    using namespace traeger;

    auto context_replier(const Context &self,
                         const char *address) -> Replier
    {
        auto [replier, error] = self.replier(address);
        if (replier)
        {
            return std::move(replier).value();
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto context_requester(const Context &self,
                           const char *address,
                           const Format &format) -> Requester
    {
        auto [requester, error] = self.requester(address, format);
        if (requester)
        {
            return std::move(requester).value();
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto context_publisher(const Context &self,
                           const char *address,
                           const Format &format) -> Publisher
    {
        auto [publisher, error] = self.publisher(address, format);
        if (publisher)
        {
            return std::move(publisher).value();
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto context_subscriber(const Context &self,
                            const char *address,
                            const std::vector<String> &topics)
    {
        auto [subscriber, error] = self.subscriber(address, topics);
        if (subscriber)
        {
            return std::move(subscriber).value();
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto subscriber_listen(const Subscriber &self,
                           const Scheduler &scheduler,
                           std::function<void(String, Variant)> callback) -> Promise
    {
        return self.listen(
            scheduler,
            [callback](const String &topic, const Value &value)
            {
                callback(topic, value_to_variant(value));
            });
    }

    auto publisher_publish(const Publisher &self,
                           const Scheduler &scheduler,
                           const String &topic,
                           const Variant &variant) -> Promise
    {
        return self.publish(scheduler, topic, value_from_variant(variant));
    }
}

auto traeger_python_register_socket_types(nb::module_ &module) -> void
{
    auto context_class = nb::class_<Context>(module, "Context");
    auto replier_class = nb::class_<Replier>(module, "Replier");
    auto requester_class = nb::class_<Requester>(module, "Requester");
    auto publisher_class = nb::class_<Publisher>(module, "Publisher");
    auto subscriber_class = nb::class_<Subscriber>(module, "Subscriber");

    context_class
        .def(nb::init<>())
        .def("replier", &context_replier)
        .def("requester", &context_requester)
        .def("publisher", &context_publisher)
        .def("subscriber", &context_subscriber);

    replier_class
        .def("reply", &Replier::reply);

    requester_class
        .def("mailbox", &Requester::mailbox);

    publisher_class
        .def("publish", &publisher_publish);

    subscriber_class
        .def("listen", &subscriber_listen);
}
