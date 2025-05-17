// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.type")
{
    using namespace traeger;

    SECTION("undefined")
    {
        const Result result;
        REQUIRE(result.type() == Result::Type::Undefined);
    }

    SECTION("value")
    {
        const Result result{Value{true}};
        REQUIRE(result.type() == Result::Type::Value);
    }

    SECTION("error")
    {
        const Result result{Error{"some error"}};
        REQUIRE(result.type() == Result::Type::Error);
    }
}
