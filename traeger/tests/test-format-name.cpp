// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/format/Format.hpp>

TEST_CASE("Format.name")
{
    SECTION("json")
    {
        const auto &format = traeger::Format::json();
        REQUIRE(format.name() == "json");
    }

    SECTION("yaml")
    {
        const auto &format = traeger::Format::yaml();
        REQUIRE(format.name() == "yaml");
    }

    SECTION("msgpack")
    {
        const auto &format = traeger::Format::msgpack();
        REQUIRE(format.name() == "msgpack");
    }
}
