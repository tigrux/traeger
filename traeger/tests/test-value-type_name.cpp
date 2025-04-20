// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <traeger/value/Value.hpp>

TEST_CASE("Value.type_name")
{
    using namespace traeger;

    SECTION("default")
    {
        Value value;
        REQUIRE(value.type_name() == "Null");
    }

    SECTION("null")
    {
        REQUIRE(Value::type_name(Value::Type::Null) == "Null");

        Value value = nullptr;
        REQUIRE(value.type_name() == "Null");
    }

    SECTION("bool")
    {
        REQUIRE(Value::type_name(Value::Type::Bool) == "Bool");

        Value value = true;
        REQUIRE(value.type_name() == "Bool");
    }

    SECTION("int")
    {
        REQUIRE(Value::type_name(Value::Type::Int) == "Int");

        Value value = std::numeric_limits<Int>::max();
        REQUIRE(value.type_name() == "Int");
    }

    SECTION("uint")
    {
        REQUIRE(Value::type_name(Value::Type::Float) == "Float");

        Value value = std::numeric_limits<UInt>::max();
        REQUIRE(value.type_name() == "UInt");
    }

    SECTION("float")
    {
        REQUIRE(Value::type_name(Value::Type::Float) == "Float");

        Value value = std::numeric_limits<Float>::max();
        REQUIRE(value.type_name() == "Float");
    }

    SECTION("string")
    {
        REQUIRE(Value::type_name(Value::Type::String) == "String");

        Value value = "Hello world";
        REQUIRE(value.type_name() == "String");
    }

    SECTION("list")
    {
        REQUIRE(Value::type_name(Value::Type::List) == "List");

        Value value = List{};
        REQUIRE(value.type_name() == "List");
    }

    SECTION("map")
    {
        REQUIRE(Value::type_name(Value::Type::Map) == "Map");

        Value value = Map{};
        REQUIRE(value.type_name() == "Map");
    }
}
