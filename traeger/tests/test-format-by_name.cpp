// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/format/Format.hpp>

TEST_CASE("Format.by_name")
{
    using namespace traeger;

    SECTION("json")
    {
        REQUIRE(Format::by_name("json") == &Format::json());
    }

    SECTION("yaml")
    {
        REQUIRE(Format::by_name("yaml") == &Format::yaml());
    }

    SECTION("msgpack")
    {
        REQUIRE(Format::by_name("msgpack") == &Format::msgpack());
    }
}
