// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.set")
{
    using namespace traeger;

    Result result;
    REQUIRE_FALSE(result.value());
    REQUIRE_FALSE(result.error());

    SECTION("value")
    {
        result = Result{Value{123}};
        REQUIRE(result.value());
        REQUIRE(*result.value() == 123);
        REQUIRE(result == Result{Value{123}});
    }

    SECTION("error")
    {
        result = Result{Error{"some error"}};
        REQUIRE(result.error());
        REQUIRE(*result.error() == "some error");
        REQUIRE(result == Result{Error{"some error"}});
    }
}