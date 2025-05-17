// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <traeger/value/Value.hpp>

TEST_CASE("Value.type")
{
    using namespace traeger;

    STATIC_REQUIRE(std::is_same_v<Null, std::nullptr_t>);
    STATIC_REQUIRE(std::is_same_v<Bool, bool>);
    STATIC_REQUIRE(std::is_same_v<Int, std::int64_t>);
    STATIC_REQUIRE(std::is_same_v<UInt, std::uint64_t>);
    STATIC_REQUIRE(std::is_same_v<Float, double>);
    STATIC_REQUIRE(std::is_same_v<String, std::string>);
    STATIC_REQUIRE(std::is_class_v<List>);
    STATIC_REQUIRE(std::is_class_v<Map>);

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
