// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <map>
#include <traeger/value/Map.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Map.iter")
{
    using namespace traeger;

    auto map = make_map("A", 3, "B", 2, "C", 1, "D", 0);

    SECTION("for-range loop")
    {
        std::map<String, Int> items;
        for (const auto &[key, value] : map)
        {
            items.emplace(key, *value.get_int());
        }
        REQUIRE(items == std::map<String, Int>{{"A", 3}, {"B", 2}, {"C", 1}, {"D", 0}});
    }

    SECTION("do-while loop")
    {
        std::map<String, Int> items;
        if (auto iter = map.begin(); iter)
        {
            do
            {
                const auto &key = iter.key();
                const auto &value = iter.value();
                items.emplace(key, *value.get_int());
            } while (iter.increment());
        }
        REQUIRE(items == std::map<String, Int>{{"A", 3}, {"B", 2}, {"C", 1}, {"D", 0}});
    }
}
