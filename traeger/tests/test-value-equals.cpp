// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <traeger/value/Value.hpp>

TEST_CASE("Value.equals")
{
    using namespace traeger;

    auto outer_values = std::vector<Value>{
        nullptr,
        true,
        1234456,
        std::numeric_limits<UInt>::max(),
        3.1416,
        "Hello world",
        make_list(10, 20, 30),
        make_map("a", 10, "b", 20, "c", 30),
    };

    auto inner_values = std::vector<Value>{
        nullptr,
        true,
        1234456,
        std::numeric_limits<UInt>::max(),
        3.1416,
        "Hello world",
        make_list(10, 20, 30),
        make_map("a", 10, "b", 20, "c", 30),
    };

    for (unsigned int outer_index = 0; outer_index < outer_values.size(); ++outer_index)
    {
        for (unsigned int inner_index = 0; inner_index < inner_values.size(); ++inner_index)
        {
            if (outer_index == inner_index)
            {
                REQUIRE(outer_values[outer_index] == inner_values[inner_index]);
            }
            else
            {
                REQUIRE_FALSE(outer_values[outer_index] == inner_values[inner_index]);
            }
        }
    }
}
