// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>

TEST_CASE("Format.encode")
{
    using namespace traeger;

    SECTION("json")
    {
        const auto &format = Format::json();

        SECTION("string")
        {
            const auto string = String{"Hello world"};

            auto [encoded_optional, encoded_error] = format.encode(string);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == "Hello world");
        }

        SECTION("list")
        {
            const auto list = make_list(nullptr,
                                        true,
                                        10,
                                        "Hello world",
                                        make_list(10, 20),
                                        make_map("x", 10, "y", 20));

            auto [encoded_optional, encoded_error] = format.encode(list);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
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
            const auto map = make_map("n", nullptr,
                                      "b", true,
                                      "i", 10,
                                      "s", "Hello world",
                                      "l", make_list(10, 20),
                                      "m", make_map("x", 10, "y", 20));

            auto [encoded_optional, encoded_error] = format.encode(map);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
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
            const auto string = String{"Hello world"};

            auto [encoded_optional, encoded_error] = format.encode(string);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == "Hello world");
        }

        SECTION("list")
        {
            const auto list = make_list(nullptr,
                                        true,
                                        10,
                                        "Hello world",
                                        make_list(10, 20),
                                        make_map("x", 10, "y", 20));

            auto [encoded_optional, encoded_error] = format.encode(list);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
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
            const auto map = make_map("n", nullptr,
                                      "b", "true",
                                      "i", "10",
                                      "s", "Hello world",
                                      "l", make_list("10", "20"),
                                      "m", make_map("x", "10", "y", "20"));

            auto [encoded_optional, encoded_error] = format.encode(map);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
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

        SECTION("string")
        {
            const auto string = String{"Hello world"};

            auto [encoded_optional, encoded_error] = format.encode(string);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
            REQUIRE(decoded_error.empty());
            REQUIRE(decoded_optional.has_value());
            REQUIRE(*decoded_optional == "Hello world");
        }

        SECTION("list")
        {
            const auto list = make_list(nullptr,
                                        true,
                                        10,
                                        "Hello world",
                                        make_list(10, 20),
                                        make_map("x", 10, "y", 20));

            auto [encoded_optional, encoded_error] = format.encode(list);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
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
            const auto map = make_map("n", nullptr,
                                      "b", true,
                                      "i", 10,
                                      "s", "Hello world",
                                      "l", make_list(10, 20),
                                      "m", make_map("x", 10, "y", 20));

            auto [encoded_optional, encoded_error] = format.encode(map);
            REQUIRE(encoded_error.empty());
            REQUIRE(encoded_optional.has_value());

            const auto [decoded_optional, decoded_error] = format.decode(*encoded_optional);
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
