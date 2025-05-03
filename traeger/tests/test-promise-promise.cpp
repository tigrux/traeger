// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Promise.hpp>

TEST_CASE("Promise.promise")
{
    using namespace traeger;

    auto scheduler = Scheduler{Threads{8}};

    auto precedent_promise = Promise{scheduler};
    auto consequent_promise = Promise{scheduler};
    consequent_promise.set_result_from_promise(precedent_promise);

    SECTION("value")
    {
        auto promise = std::promise<Value>{};
        consequent_promise.then(
            [&promise](const Value &value) -> Result
            {
                promise.set_value(value);
                return Result{};
            });

        precedent_promise.set_result(Value{123});
        REQUIRE(promise.get_future().get() == Value{123});
    }

    SECTION("error")
    {
        auto promise = std::promise<Error>{};
        consequent_promise.fail(
            [&promise](const Error &error) -> void
            {
                promise.set_value(error);
            });

        precedent_promise.set_result(Error{"some error"});
        REQUIRE(promise.get_future().get() == Error{"some error"});
    }
}