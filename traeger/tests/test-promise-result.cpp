// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Promise.hpp>

TEST_CASE("Promise.result")
{
    using namespace traeger;

    auto scheduler = Scheduler{Threads{8}};

    auto promise = Promise{scheduler};
    REQUIRE_FALSE(promise.has_result());

    SECTION("value")
    {
        REQUIRE(promise.set_result(Value{123}));
        REQUIRE(promise.has_result());
        REQUIRE(promise.result().value());
        REQUIRE(*promise.result().value() == 123);

        REQUIRE_FALSE(promise.set_result(Value{456}));
        REQUIRE(*promise.result().value() == 123);
    }

    SECTION("error")
    {
        REQUIRE(promise.set_result(Error{"some error"}));
        REQUIRE(promise.has_result());
        REQUIRE(promise.result().error());
        REQUIRE(*promise.result().error() == "some error");

        REQUIRE_FALSE(promise.set_result(Error{"another error"}));
        REQUIRE(*promise.result().error() == "some error");
    }
}