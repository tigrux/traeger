// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <array>
#include <traeger/value/Map.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Map.size")
{
    using namespace traeger;

    auto map = Map{};
    REQUIRE(map.size() == 0);

    SECTION("size increases one at a time")
    {
        auto keys = std::array{"A", "B", "C", "D", "E"};
        for (unsigned int i = 0; i < 5; ++i)
        {
            map.set(keys[i], i + 1);
            REQUIRE(map.size() == i + 1);
        }
    }
    REQUIRE(Value{map} == make_map("A", 1, "B", 2, "C", 3, "D", 4, "E", 5));
}
