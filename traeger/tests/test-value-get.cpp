// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Value.get")
{
    using namespace traeger;

    Value value;

    SECTION("null")
    {
        value = nullptr;
        REQUIRE(*value.get_null() == nullptr);
    }

    SECTION("bool")
    {
        value = true;
        REQUIRE(*value.get_bool() == true);

        SECTION("from a string that represents a bool")
        {
            value = "true";
            REQUIRE(*value.get_bool() == true);
        }
    }

    SECTION("int")
    {
        value = 123456;
        REQUIRE(*value.get_int() == 123456);

        SECTION("from a uint that can converted to int")
        {
            value = 123456u;
            REQUIRE(*value.get_int() == 123456);
        }

        SECTION("from a string that represents an int")
        {
            value = "123456";
            REQUIRE(*value.get_int() == 123456);
        }
    }

    SECTION("uint")
    {
        value = 123456u;
        REQUIRE(*value.get_uint() == 123456);

        SECTION("from an int that can converted to uint")
        {
            value = 123456;
            REQUIRE(*value.get_uint() == 123456);
        }

        SECTION("from a uint that cannot converted to int")
        {
            value = std::numeric_limits<UInt>::max();
            REQUIRE(*value.get_uint() == std::numeric_limits<UInt>::max());
            REQUIRE_FALSE(value.get_int().has_value());
        }

        SECTION("from a string that represents an uint")
        {
            value = "123456";
            REQUIRE(*value.get_uint() == 123456);
        }
    }

    SECTION("float")
    {
        value = 3.1416;
        REQUIRE(*value.get_float() == 3.1416);

        SECTION("from a string that represents a float")
        {
            value = "3.1416";
            REQUIRE(*value.get_float() == 3.1416);
        }
    }

    SECTION("string")
    {
        String str = "ABCDEF";
        value = str;
        REQUIRE(*value.get_string() == str);
    }

    SECTION("get_list")
    {
        value = make_list(true, 123, "Hello");
        SECTION("copies do not affect")
        {
            auto list = *value.get_list();
            list.append(3.1416);
            REQUIRE(list.size() == 4);
            REQUIRE(value.get_list()->size() == 3);
        }
        Bool b{};
        Int i{};
        String s{};
        value.get_list()->unpack(b, i, s);
        REQUIRE(b == true);
        REQUIRE(i == 123);
        REQUIRE(s == "Hello");
    }

    SECTION("get_map")
    {
        value = make_map("b", true, "i", 123, "s", "Hello");
        SECTION("copies do not affect")
        {
            auto map = *value.get_map();
            map.set("f", 3.1416);
            REQUIRE(map.size() == 4);
            REQUIRE(value.get_map()->size() == 3);
        }
        Bool b{};
        Int i{};
        String s{};
        value.get_map()->get("b", b, "i", i, "s", s);
        REQUIRE(b == true);
        REQUIRE(i == 123);
        REQUIRE(s == "Hello");
    }
}
