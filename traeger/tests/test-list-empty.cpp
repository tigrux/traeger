// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.empty")
{
    using namespace traeger;

    auto list = List{};
    REQUIRE(list.empty());
    REQUIRE(list.size() == 0);

    SECTION("copies are not affected")
    {
        auto list2 = list;
        list.append(10);
        REQUIRE_FALSE(list.empty());
        REQUIRE(list2.empty());
    }
}
