// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.size")
{
    using namespace traeger;

    auto list = List{};
    REQUIRE(list.empty());

    SECTION("size increases one at a time")
    {
        for (unsigned int i = 1; i <= 5; ++i)
        {
            list.append(i);
            REQUIRE(list.size() == i);
        }
        REQUIRE(list == make_list(1, 2, 3, 4, 5));
    }

    SECTION("size increases at once")
    {
        list.resize(5);
        REQUIRE(list.size() == 5);
        REQUIRE(list == make_list(nullptr, nullptr, nullptr, nullptr, nullptr));
    }
}
