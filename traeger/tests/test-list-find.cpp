// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.find")
{
    using namespace traeger;

    auto list = make_list(true, 123, "Hello");
    REQUIRE(*list.find(0) == true);
    REQUIRE(*list.find(1) == 123);
    REQUIRE(*list.find(2) == "Hello");

    SECTION("invalid index")
    {
        REQUIRE_FALSE(list.find(3));
    }

    SECTION("index just added")
    {
        list.append(3.1416);
        REQUIRE(*list.find(3) == 3.1416);
    }
}
