// SPDX-License-Identifier: BSL-1.0

#include <nanobind/stl/function.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>

#include <cstdint>
#include <chrono>
#include <utility>
#include <ratio>
#include <cmath>
#include <functional>

#include <traeger/value/Variant.hpp>
#include <traeger/actor/Mailbox.hpp>
#include <traeger/actor/Promise.hpp>
#include <traeger/actor/Queue.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/StatelessActor.hpp>

#include "traeger_python.hpp"

namespace
{
    namespace nb = nanobind;
    using namespace traeger;

    auto result_from_variant(Variant variant) -> Result
    {
        return {value_from_variant(std::move(variant))};
    }

    auto result_from_error(String error) -> Result
    {
        return {Error{std::move(error)}};
    }

    auto result_repr(const Result &self)
    {
        return type_repr("traeger.Result", self);
    }

    auto scheduler_init(Scheduler *self,
                        unsigned int threads_count)
    {
        new (self) Scheduler{Threads{threads_count}};
    }

    auto scheduler_schedule(Scheduler &self,
                            Work work) -> void
    {
        self.schedule(std::move(work));
    }

    auto scheduler_schedule_delayed(Scheduler &self,
                                    Float delay,
                                    Work work) -> void
    {
        self.schedule_delayed(to_microseconds(delay), std::move(work));
    }

    auto mailbox_send(const Mailbox &self,
                      const Scheduler &scheduler,
                      const String &name,
                      nb::args args) -> Promise
    {
        List arguments;
        Variant variant;
        for (const auto &handle : args)
        {
            if (nb::try_cast<Variant>(handle, variant))
            {
                arguments.append(value_from_variant(std::move(variant)));
            }
            else
            {
                String error = "'" + std::string(nb::type_name(handle.type()).c_str()) +
                               "' object is not convertible to traeger.Value";
                throw nb::type_error(error.c_str());
            }
        }
        return self.send(scheduler, name, arguments);
    }

    using ThenVariant = std::variant<Variant, Result, Promise>;

    auto promise_then_result(Promise &self,
                             std::function<Variant(Variant)> then_callback) -> Promise
    {
        return self.then(
            [then_callback](const Value &value) -> Result
            {
                try
                {
                    return {value_from_variant(then_callback(value_to_variant(value)))};
                }
                catch (const std::exception &e)
                {
                    return {Error{e.what()}};
                }
            });
    }

    auto promise_then_promise(Promise &self,
                              std::function<Promise(Variant)> then_callback) -> Promise
    {
        return self.then(
            [then_callback](const Value &value) -> Promise
            {
                return then_callback(value_to_variant(value));
            });
    }

    auto promise_fail(Promise &self,
                      std::function<void(String)> error_callback) -> void
    {
        self.fail(
            [error_callback](const Error &error) -> void
            {
                error_callback(error);
            });
    }

    auto promise_repr(const Promise &self) -> String
    {
        return type_repr("traeger.Promise", self.result());
    }

    auto promise_set_result(const Promise &self,
                            const Result &result) -> void
    {
        self.set_result(result);
    }

    auto queue_iter(const Queue &self) -> const Queue &
    {
        return self;
    }

    auto queue_push_variant(const Queue &self,
                            const Variant &variant) -> void
    {
        self.push(value_from_variant(variant));
    }

    auto queue_next(const Queue &self) -> Variant
    {
        if (auto optional = self.pop(); optional)
        {
            return value_to_variant(optional.value());
        }
        throw nb::stop_iteration();
    }
}

auto traeger_python_register_actor_types(nb::module_ &module) -> void
{
    auto result_class = nb::class_<Result>(module, "Result");
    auto scheduler_class = nb::class_<Scheduler>(module, "Scheduler");
    auto mailbox_class = nb::class_<Mailbox>(module, "Mailbox");
    auto actor_class = nb::class_<StatelessActor>(module, "StatelessActor");
    auto promise_class = nb::class_<Promise>(module, "Promise");
    auto queue_class = nb::class_<Queue>(module, "Queue");

    result_class
        .def(nb::init<>())
        .def("__repr__", &result_repr)
        .def_static("from_value", &result_from_variant, nb::arg("value").none())
        .def_static("from_error", &result_from_error);

    scheduler_class
        .def("__init__", &scheduler_init, nb::arg("threads_count"))
        .def("count", &Scheduler::count, nb::call_guard<nb::gil_scoped_release>())
        .def("schedule", &scheduler_schedule)
        .def("schedule_delayed", &scheduler_schedule_delayed);

    mailbox_class
        .def("send", &mailbox_send);

    actor_class
        .def(nb::init<>())
        .def("define_reader", &StatelessActor::define_reader)
        .def("define_writer", &StatelessActor::define_writer)
        .def("mailbox", &StatelessActor::mailbox);

    promise_class
        .def("__repr__", &promise_repr)
        .def("set", promise_set_result)
        .def("then_result", &promise_then_result)
        .def("then_promise", &promise_then_promise)
        .def("fail", &promise_fail);

    queue_class
        .def(nb::init<>())
        .def("closed", &Queue::closed)
        .def("__len__", &Queue::count)
        .def("push", &queue_push_variant, nb::arg("value").none())
        .def("__iter__", &queue_iter)
        .def("__next__", &queue_next)
        .def("close", &Queue::close);
}
