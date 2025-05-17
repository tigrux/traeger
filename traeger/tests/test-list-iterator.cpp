// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.iter")
{
    using namespace traeger;

    const auto list = make_list("A3", "B2", "C1", "D0");

    SECTION("for-range loop")
    {
        std::vector<String> values;
        for (const auto &value : list)
        {
            values.push_back(*value.get_string());
        }
        REQUIRE(values == std::vector<String>{"A3", "B2", "C1", "D0"});
    }

    SECTION("do-while loop")
    {
        std::vector<String> values;
        if (auto iter = list.begin(); iter)
        {
            do
            {
                const auto &value = iter.value();
                values.push_back(*value.get_string());
            } while (iter.increment());
        }
        REQUIRE(values == std::vector<String>{"A3", "B2", "C1", "D0"});
    }
}
