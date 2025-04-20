// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <traeger/actor/Result.hpp>

TEST_CASE("Result.equals")
{
    using namespace traeger;

    auto outer_results = std::vector<Result>{
        Value{nullptr},
        Value{true},
        Value{1234456},
        Value{std::numeric_limits<UInt>::max()},
        Value{3.1416},
        Value{"Hello world"},
        Value{make_list(10, 20, 30)},
        Value{make_map("a", 10, "b", 20, "c", 30)},
        Error{"some error"},
        Error{"another error"}};

    auto inner_results = std::vector<Result>{
        Value{nullptr},
        Value{true},
        Value{1234456},
        Value{std::numeric_limits<UInt>::max()},
        Value{3.1416},
        Value{"Hello world"},
        Value{make_list(10, 20, 30)},
        Value{make_map("a", 10, "b", 20, "c", 30)},
        Error{"some error"},
        Error{"another error"}};

    for (unsigned int outer_index = 0; outer_index < outer_results.size(); ++outer_index)
    {
        for (unsigned int inner_index = 0; inner_index < inner_results.size(); ++inner_index)
        {
            if (outer_index == inner_index)
            {
                REQUIRE(outer_results[outer_index] == inner_results[inner_index]);
            }
            else
            {
                REQUIRE_FALSE(outer_results[outer_index] == inner_results[inner_index]);
            }
        }
    }
}
