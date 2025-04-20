// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Scheduler.hpp>

TEST_CASE("Scheduler.count")
{
    using namespace traeger;

    const auto scheduler = Scheduler{Threads{8}};
    REQUIRE(scheduler.count() == 0);

    SECTION("references")
    {
        auto reference1 = Scheduler{scheduler};
        REQUIRE(scheduler.count() == 1);

        auto reference2 = Scheduler{scheduler};
        REQUIRE(scheduler.count() == 2);

        auto reference3 = Scheduler{scheduler};
        REQUIRE(scheduler.count() == 3);
    }

    REQUIRE(scheduler.count() == 0);
}