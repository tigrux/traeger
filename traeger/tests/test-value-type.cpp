// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <traeger/value/Value.hpp>

TEST_CASE("Value.type")
{
    using namespace traeger;

    STATIC_REQUIRE(std::is_same<Null, std::nullptr_t>::value);
    STATIC_REQUIRE(std::is_same<Bool, bool>::value);
    STATIC_REQUIRE(std::is_same<Int, std::int64_t>::value);
    STATIC_REQUIRE(std::is_same<UInt, std::uint64_t>::value);
    STATIC_REQUIRE(std::is_same<Float, double>::value);
    STATIC_REQUIRE(std::is_same<String, std::string>::value);
    STATIC_REQUIRE(std::is_class<List>::value);
    STATIC_REQUIRE(std::is_class<Map>::value);

    SECTION("default")
    {
        Value value;
        REQUIRE(value.type() == Value::Type::Null);
    }

    SECTION("null")
    {
        Value value = nullptr;
        REQUIRE(value.type() == Value::Type::Null);
    }

    SECTION("bool")
    {
        Value value = true;
        REQUIRE(value.type() == Value::Type::Bool);
    }

    SECTION("int")
    {
        Value value = std::numeric_limits<Int>::max();
        REQUIRE(value.type() == Value::Type::Int);
    }

    SECTION("uint")
    {
        Value value = std::numeric_limits<UInt>::max();
        REQUIRE(value.type() == Value::Type::UInt);
    }

    SECTION("float")
    {
        Value value = std::numeric_limits<Float>::max();
        REQUIRE(value.type() == Value::Type::Float);
    }

    SECTION("string")
    {
        Value value = "Hello world";
        REQUIRE(value.type() == Value::Type::String);
    }

    SECTION("list")
    {
        Value value = List{};
        REQUIRE(value.type() == Value::Type::List);
    }

    SECTION("map")
    {
        Value value = Map{};
        REQUIRE(value.type() == Value::Type::Map);
    }
}
