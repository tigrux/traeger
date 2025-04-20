// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.type_name")
{
    using namespace traeger;

    SECTION("undefined")
    {
        REQUIRE(Result::type_name(Result::Type::Undefined) == "Undefined");

        Result result;
        REQUIRE(result.type_name() == "Undefined");
    }

    SECTION("value")
    {
        REQUIRE(Result::type_name(Result::Type::Value) == "Value");

        Result result{Value{true}};
        REQUIRE(result.type_name() == "Value");
    }

    SECTION("error")
    {
        REQUIRE(Result::type_name(Result::Type::Error) == "Error");

        Result result{Error{"some error"}};
        REQUIRE(result.type_name() == "Error");
    }
}
