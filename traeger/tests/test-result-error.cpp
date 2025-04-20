// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.error")
{
    using namespace traeger;

    SECTION("undefined")
    {
        Result result;
        REQUIRE_FALSE(result.error());
    }

    SECTION("value")
    {
        Result result{Value{123}};
        REQUIRE_FALSE(result.error());
    }

    SECTION("error")
    {
        Result result{Error{"some error"}};
        REQUIRE(result.error());
        REQUIRE(*result.error() == "some error");
    }
}