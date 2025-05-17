// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/Map.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Map.empty")
{
    using namespace traeger;

    auto map = Map{};
    REQUIRE(map.empty());
    REQUIRE(map.empty());

    SECTION("copies are not affected")
    {
        const auto map2 = map;
        map.set("i", 10);
        REQUIRE_FALSE(map.empty());
        REQUIRE(map2.empty());
    }
}
