// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/Map.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Map.find")
{
    using namespace traeger;

    auto map = make_map("b", true, "i", 123, "s", "Hello");
    REQUIRE(*map.find("b") == true);
    REQUIRE(*map.find("i") == 123);
    REQUIRE(*map.find("s") == "Hello");

    SECTION("invalid key")
    {
        REQUIRE_FALSE(map.find("f"));
    }

    SECTION("key just added")
    {
        map.set("f", 3.1416);
        REQUIRE(*map.find("f") == 3.1416);
    }
}
