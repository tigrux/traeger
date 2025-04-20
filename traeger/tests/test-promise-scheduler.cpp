// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Promise.hpp>

TEST_CASE("Promise.scheduler")
{
    using namespace traeger;

    auto scheduler = Scheduler{Threads{8}};
    REQUIRE(scheduler.count() == 0);

    SECTION("promises")
    {
        auto promise1 = Promise{scheduler};
        REQUIRE(promise1.scheduler().count() == 1);

        auto promise2 = Promise{promise1.scheduler()};
        REQUIRE(promise2.scheduler().count() == 2);

        auto promise3 = Promise{promise2.scheduler()};
        REQUIRE(promise3.scheduler().count() == 3);

        auto promise4 = Promise{promise3.scheduler()};
        REQUIRE(promise4.scheduler().count() == 4);
        REQUIRE(scheduler.count() == 4);
    }

    REQUIRE(scheduler.count() == 0);
}