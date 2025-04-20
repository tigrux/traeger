// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <chrono>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Promise.hpp>

TEST_CASE("Promise.then")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    auto scheduler = Scheduler{Threads{8}};
    auto precedent_promise = Promise{scheduler};

    auto first_promise = std::promise<Value>{};
    auto second_promise = std::promise<Value>{};

    SECTION("result")
    {
        precedent_promise
            .then(
                [&first_promise](const Value &value) -> Result
                {
                    first_promise.set_value(value);
                    return Value{2000};
                })
            .then(
                [&second_promise](const Value &value) -> Result
                {
                    second_promise.set_value(value);
                    return Result{};
                });

        precedent_promise.set_result(Value{1000});
        REQUIRE(first_promise.get_future().get() == Value{1000});
        REQUIRE(second_promise.get_future().get() == Value{2000});
    }

    SECTION("promise")
    {
        precedent_promise
            .then(
                [&first_promise, scheduler](const Value &value) -> Promise
                {
                    first_promise.set_value(value);
                    auto promise = Promise{scheduler};
                    scheduler.schedule_delayed(
                        10ms,
                        [promise]
                        {
                            promise.set_result(Value{2000});
                        });
                    return promise;
                })
            .then(
                [&second_promise](const Value &value) -> Result
                {
                    second_promise.set_value(value);
                    return Result{};
                });

        precedent_promise.set_result(Value{1000});
        REQUIRE(first_promise.get_future().get() == Value{1000});
        REQUIRE(second_promise.get_future().get() == Value{2000});
    }
}