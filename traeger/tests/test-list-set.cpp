// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.set")
{
    using namespace traeger;

    auto list = make_list(10, 20, 30);
    REQUIRE(list.size() == 3);

    SECTION("first position with a temporary")
    {
        REQUIRE(*list.find(0) == 10);
        list.set(0, 100);
        REQUIRE(*list.find(0) == 100);
    }

    SECTION("second position with a const-ref")
    {
        REQUIRE(*list.find(1) == 20);
        const auto value = Value{200};
        list.set(1, value);
        REQUIRE(*list.find(1) == 200);
    }

    SECTION("last position with a move-ref")
    {
        REQUIRE(*list.find(-1) == 30);
        list.set(-1, Value{300});
        REQUIRE(*list.find(-1) == 300);
    }

    SECTION("invalid position")
    {
        REQUIRE_FALSE(list.find(3));
        REQUIRE_FALSE(list.set(3, 400));
    }
}
