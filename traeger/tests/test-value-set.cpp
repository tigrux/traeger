// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Value.set")
{
    using namespace traeger;

    Value value;

    SECTION("temporary")
    {
        value = true;
        REQUIRE(value == true);

        SECTION("from another temporary")
        {
            value = 10;
            REQUIRE(value == 10);
        }
    }

    SECTION("const-ref")
    {
        const Value v1{true};
        value = v1;
        REQUIRE(value == true);

        SECTION("from another const-ref")
        {
            const Value v2{10};
            value = v2;
            REQUIRE(value == 10);
        }
    }

    SECTION("move-ref")
    {
        value = Value{true};
        REQUIRE(value == true);

        SECTION("from another move-ref")
        {
            value = Value{10};
            REQUIRE(value == 10);
        }
    }
}
