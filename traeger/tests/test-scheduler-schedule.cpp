// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>

TEST_CASE("Scheduler.schedule")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    auto promise = std::promise<int>{};
    const auto scheduler = Scheduler{Threads{8}};

    scheduler.schedule(
        [&promise]
        {
            promise.set_value(123);
        });

    REQUIRE(promise.get_future().get() == 123);
}
