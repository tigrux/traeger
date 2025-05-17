// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.value")
{
    using namespace traeger;

    SECTION("undefined")
    {
        const Result result;
        REQUIRE_FALSE(result.value());
    }

    SECTION("value")
    {
        const Result result{Value{123}};
        REQUIRE(result.value());
        REQUIRE(*result.value() == 123);
    }

    SECTION("error")
    {
        const Result result{Error{"some error"}};
        REQUIRE_FALSE(result.value());
    }
}