// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <chrono>
#include <thread>
#include <traeger/actor/StatelessActor.hpp>

TEST_CASE("StatelessActor.define")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    const auto scheduler = Scheduler{Threads{8}};
    const auto actor = StatelessActor{};

    SECTION("reader")
    {
        actor.define_reader(
            "sum",
            [](const List &arguments) -> Result
            {
                Int sum = 0;
                for (const auto &value : arguments)
                {
                    if (auto arg_int = value.get_int(); arg_int)
                    {
                        sum += *arg_int;
                    }
                }
                std::this_thread::sleep_for(10ms);
                return Result{Value{sum}};
            });

        const auto mailbox = actor.mailbox();
        auto promise1 = std::promise<Value>{};
        mailbox
            .send(scheduler, "sum", make_list(1000, 100, 10, 1))
            .then(
                [&promise1](const Value &value)
                {
                    promise1.set_value(value);
                    return Result{};
                });

        auto promise2 = std::promise<Value>{};
        mailbox
            .send(scheduler, "sum", make_list(2000, 200, 20, 2))
            .then(
                [&promise2](const Value &value)
                {
                    promise2.set_value(value);
                    return Result{};
                });

        auto promise3 = std::promise<Value>{};
        mailbox
            .send(scheduler, "sum", make_list(3000, 300, 30, 3))
            .then(
                [&promise3](const Value &value)
                {
                    promise3.set_value(value);
                    return Result{};
                });

        auto promise4 = std::promise<Value>{};
        mailbox
            .send(scheduler, "sum", make_list(4000, 400, 40, 4))
            .then(
                [&promise4](const Value &value)
                {
                    promise4.set_value(value);
                    return Result{};
                });

        REQUIRE(promise1.get_future().get() == 1111);
        REQUIRE(promise2.get_future().get() == 2222);
        REQUIRE(promise3.get_future().get() == 3333);
        REQUIRE(promise4.get_future().get() == 4444);
    }

    SECTION("writer")
    {
        const auto todo_list = std::make_shared<List>();
        actor.define_writer(
            "add-todo",
            [todo_list](const List &arguments) -> Result
            {
                for (const auto &value : arguments)
                {
                    todo_list->append(value);
                }
                return Result{};
            });
        actor.define_reader(
            "get-todo",
            [todo_list](const List &) -> Result
            {
                return Result{Value{*todo_list}};
            });

        const auto mailbox = actor.mailbox();
        for (auto item : {"lettuce", "milk", "eggs", "soda"})
        {
            mailbox.send(scheduler, "add-todo", make_list(item));
        }

        auto value_promise = std::promise<Value>{};
        mailbox
            .send(scheduler, "get-todo", List{})
            .then(
                [&value_promise](const Value &value) -> Result
                {
                    value_promise.set_value(value);
                    return Result{};
                });
        REQUIRE(value_promise.get_future().get() == make_list("lettuce", "milk", "eggs", "soda"));
    }
}
