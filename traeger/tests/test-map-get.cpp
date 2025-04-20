// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/Map.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("Map.get")
{
    using namespace traeger;

    SECTION("types match")
    {
        auto map = make_map("b", true, "i", 10, "f", 3.1416);
        Bool b{};
        Int i{};
        Float f{};
        auto [ok, error] = map.get("b", b, "i", i, "f", f);
        REQUIRE(ok);
        REQUIRE(b == true);
        REQUIRE(i == 10);
        REQUIRE(f == 3.1416);
    }

    SECTION("types do not match")
    {
        auto map = make_map("b", nullptr, "i", nullptr, "f", nullptr);
        Bool b{};
        Int i{};
        Float f{};
        auto [ok, error] = map.get("b", b, "i", i, "f", f);
        REQUIRE_FALSE(ok);
        REQUIRE_FALSE(error.empty());
    }

    SECTION("size does not match")
    {
        auto map = make_map("b", true, "i", 10);
        Bool b{};
        Int i{};
        Float f{};
        auto [ok, error] = map.get("b", b, "i", i, "f", f);
        REQUIRE_FALSE(ok);
        REQUIRE_FALSE(error.empty());
    }
}
