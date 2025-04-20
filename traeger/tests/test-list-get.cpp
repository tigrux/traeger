// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/value/List.hpp>
#include <traeger/value/Value.hpp>

TEST_CASE("List.get")
{
    using namespace traeger;

    SECTION("types match")
    {
        auto list = make_list(true, 10, 3.1416);
        Bool b{};
        Int i{};
        Float f{};
        auto [ok, error] = list.get(b, i, f);
        REQUIRE(ok);
        REQUIRE(b == true);
        REQUIRE(i == 10);
        REQUIRE(f == 3.1416);
    }

    SECTION("types do not match")
    {
        auto list = make_list(nullptr, nullptr, nullptr);
        Bool b{};
        Int i{};
        Float f{};
        auto [ok, error] = list.get(b, i, f);
        REQUIRE_FALSE(ok);
        REQUIRE_FALSE(error.empty());
    }

    SECTION("size does not match")
    {
        auto list = make_list(true, 10);
        Bool b{};
        Int i{};
        Float f{};
        auto [ok, error] = list.get(b, i, f);
        REQUIRE_FALSE(ok);
        REQUIRE_FALSE(error.empty());
    }
}
