// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Promise.hpp>

TEST_CASE("Promise.fail")
{
    using namespace traeger;

    auto scheduler = Scheduler{Threads{8}};
    auto precedent_promise = Promise{scheduler};

    auto error_promise = std::promise<Error>{};

    SECTION("error-fail")
    {
        precedent_promise
            .fail(
                [&error_promise](const Error &error)
                {
                    error_promise.set_value(error);
                });

        precedent_promise.set_result(Result{Error{"some error"}});
        REQUIRE(error_promise.get_future().get() == Error{"some error"});
    }

    SECTION("value-then-error")
    {
        auto value_promise = std::promise<Value>{};
        precedent_promise
            .then(
                [&value_promise](const Value &value) -> Result
                {
                    value_promise.set_value(value);
                    return Result{Error{"some error"}};
                })
            .fail(
                [&error_promise](const Error &error)
                {
                    error_promise.set_value(error);
                });

        precedent_promise.set_result(Result{Value{123}});
        REQUIRE(value_promise.get_future().get() == Value{123});
        REQUIRE(error_promise.get_future().get() == Error{"some error"});
    }

    SECTION("error-fallthrough")
    {
        bool first_then_visited = false;
        bool second_then_visited = false;
        precedent_promise
            .then(
                [&first_then_visited](const Value &) -> Result
                {
                    first_then_visited = true;
                    return Result{Value{123}};
                })
            .then(
                [&second_then_visited](const Value &) -> Result
                {
                    second_then_visited = true;
                    return Result{Value{456}};
                })
            .fail(
                [&error_promise](const Error &error)
                {
                    error_promise.set_value(error);
                });

        precedent_promise.set_result(Result{Error{"some error"}});
        REQUIRE(error_promise.get_future().get() == Error{"some error"});
        REQUIRE_FALSE(first_then_visited);
        REQUIRE_FALSE(second_then_visited);
    }
}