// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.append")
{
    using namespace traeger;

    auto list = List{};
    REQUIRE(list.size() == 0);

    SECTION("temporary")
    {
        list.append(true);
        REQUIRE(list == make_list(true));

        SECTION("copies are not affected")
        {
            auto list2 = list;
            list.append(123);
            REQUIRE(list == make_list(true, 123));
            REQUIRE(list2 == make_list(true));
        }
    }

    SECTION("const-ref")
    {
        Value value_1{true};
        list.append(value_1);
        REQUIRE(list == make_list(true));

        SECTION("copies are not affected")
        {
            auto list2 = list;
            Value value_2{123};
            list.append(value_2);
            REQUIRE(list == make_list(true, 123));
            REQUIRE(list2 == make_list(true));
        }
    }

    SECTION("move-ref")
    {
        list.append(Value{true});
        REQUIRE(list.size() == 1);

        SECTION("copies are not affected")
        {
            auto list2 = list;
            list.append(Value{123});
            REQUIRE(list == make_list(true, 123));
            REQUIRE(list2 == make_list(true));
        }
    }
}
