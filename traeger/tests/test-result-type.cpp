// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.type")
{
    using namespace traeger;

    SECTION("undefined")
    {
        Result result;
        REQUIRE(result.type() == Result::Type::Undefined);
    }

    SECTION("value")
    {
        Result result{Value{true}};
        REQUIRE(result.type() == Result::Type::Value);
    }

    SECTION("error")
    {
        Result result{Error{"some error"}};
        REQUIRE(result.type() == Result::Type::Error);
    }
}
