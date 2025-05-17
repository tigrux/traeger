// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/Map.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Map.set")
{
    using namespace traeger;

    auto map = Map{};
    REQUIRE(map.empty());

    SECTION("temporary")
    {
        map.set("b", true);
        REQUIRE(map == make_map("b", true));

        SECTION("copies are not affected")
        {
            auto map2 = map;
            map.set("i", 123);
            REQUIRE(map == make_map("b", true, "i", 123));
            REQUIRE(map2 == make_map("b", true));
        }
    }

    SECTION("const-ref")
    {
        Value value_1{true};
        map.set("b", value_1);
        REQUIRE(map == make_map("b", true));

        SECTION("copies are not affected")
        {
            auto map2 = map;
            Value value_2{123};
            map.set("i", value_2);
            REQUIRE(map == make_map("b", true, "i", 123));
            REQUIRE(map2 == make_map("b", true));
        }
    }

    SECTION("move-ref")
    {
        map.set("b", Value{true});
        REQUIRE(map.size() == 1);

        SECTION("copies are not affected")
        {
            auto map2 = map;
            map.set("i", Value{123});
            REQUIRE(map == make_map("b", true, "i", 123));
            REQUIRE(map2 == make_map("b", true));
        }
    }
}
