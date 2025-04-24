// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.resize")
{
    using namespace traeger;

    auto list = make_list(10, 20, 30);
    REQUIRE(list.size() == 3);

    SECTION("greater size appends null values")
    {
        list.resize(5);
        REQUIRE(list.size() == 5);
        REQUIRE(list == make_list(10, 20, 30, nullptr, nullptr));
    }

    SECTION("smaller size removes values")
    {
        list.resize(2);
        REQUIRE(list == make_list(10, 20));
        REQUIRE(list.size() == 2);
    }

    SECTION("same size has no effect")
    {
        list.resize(3);
        REQUIRE(list.size() == 3);
        REQUIRE(list == make_list(10, 20, 30));
    }
}
