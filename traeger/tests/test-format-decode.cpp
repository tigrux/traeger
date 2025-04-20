// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>

TEST_CASE("Format.decode")
{
    using namespace traeger;

    SECTION("json")
    {
        const auto &format = Format::json();

        SECTION("string")
        {
            auto [decoded_optional, decoded_error] = format.decode("\"Hello world\"");
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == String("Hello world"));
        }

        SECTION("list")
        {
            auto [decoded_optional, decoded_error] = format.decode("[null,true,10,\"Hello world\",[10,20],{\"x\":10,\"y\":20}]");
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == make_list(nullptr,
                                                   true,
                                                   10,
                                                   "Hello world",
                                                   make_list(10, 20),
                                                   make_map("x", 10, "y", 20)));
        }

        SECTION("map")
        {
            auto [decoded_optional, decoded_error] = format.decode("{\"b\":true,\"i\":10,\"l\":[10,20],\"m\":{\"x\":10,\"y\":20},\"n\":null,\"s\":\"Hello world\"}");
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == make_map("n", nullptr,
                                                  "b", true,
                                                  "i", 10,
                                                  "s", "Hello world",
                                                  "l", make_list(10, 20),
                                                  "m", make_map("x", 10, "y", 20)));
        }
    }

    SECTION("yaml")
    {
        const auto &format = Format::yaml();

        SECTION("string")
        {
            auto [decoded_optional, decoded_error] = format.decode("Hello world");
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == String("Hello world"));
        }

        SECTION("list")
        {
            auto [decoded_optional, decoded_error] = format.decode("- ~\n"
                                                                   "- true\n"
                                                                   "- 10\n"
                                                                   "- Hello world\n"
                                                                   "-\n"
                                                                   "  - 10\n"
                                                                   "  - 20\n"
                                                                   "- y: 20\n"
                                                                   "  x: 10");
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == make_list(nullptr,
                                                   "true",
                                                   "10",
                                                   "Hello world",
                                                   make_list("10", "20"),
                                                   make_map("x", "10", "y", "20")));
        }

        SECTION("map")
        {
            auto [decoded_optional, decoded_error] = format.decode("l:\n"
                                                                   "  - 10\n"
                                                                   "  - 20\n"
                                                                   "b: true\n"
                                                                   "m:\n"
                                                                   "  y: 20\n"
                                                                   "  x: 10\n"
                                                                   "s: Hello world\n"
                                                                   "n: ~\n"
                                                                   "i: 10");
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == make_map("n", nullptr,
                                                  "b", "true",
                                                  "i", "10",
                                                  "s", "Hello world",
                                                  "l", make_list("10", "20"),
                                                  "m", make_map("x", "10", "y", "20")));
        }
    }

    SECTION("msgpack")
    {
        const auto &format = Format::msgpack();

        auto bytes = [](std::vector<std::uint8_t> &&v)
        {
            return String{v.begin(), v.end()};
        };

        SECTION("string")
        {
            auto [decoded_optional, decoded_error] = format.decode(bytes({0xAB, 0x48, 0x65, 0x6C,
                                                                          0x6C, 0x6F, 0x20, 0x77,
                                                                          0x6F, 0x72, 0x6C, 0x64}));
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == String("Hello world"));
        }

        SECTION("list")
        {
            auto [decoded_optional, decoded_error] = format.decode(bytes({0x96, 0xC0, 0xC3, 0x0A,
                                                                          0xAB, 0x48, 0x65, 0x6C,
                                                                          0x6C, 0x6F, 0x20, 0x77,
                                                                          0x6F, 0x72, 0x6C, 0x64,
                                                                          0x92, 0x0A, 0x14, 0x82,
                                                                          0xA1, 0x79, 0x14, 0xA1,
                                                                          0x78, 0x0A}));
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == make_list(nullptr,
                                                   true,
                                                   10,
                                                   "Hello world",
                                                   make_list(10, 20),
                                                   make_map("x", 10, "y", 20)));
        }

        SECTION("map")
        {
            auto [decoded_optional, decoded_error] = format.decode(bytes({0x86, 0xA1, 0x6C, 0x92,
                                                                          0x0A, 0x14, 0xA1, 0x62,
                                                                          0xC3, 0xA1, 0x6D, 0x82,
                                                                          0xA1, 0x79, 0x14, 0xA1,
                                                                          0x78, 0x0A, 0xA1, 0x73,
                                                                          0xAB, 0x48, 0x65, 0x6C,
                                                                          0x6C, 0x6F, 0x20, 0x77,
                                                                          0x6F, 0x72, 0x6C, 0x64,
                                                                          0xA1, 0x6E, 0xC0, 0xA1,
                                                                          0x69, 0x0A}));
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == make_map("n", nullptr,
                                                  "b", true,
                                                  "i", 10,
                                                  "s", "Hello world",
                                                  "l", make_list(10, 20),
                                                  "m", make_map("x", 10, "y", 20)));
        }
    }
}
