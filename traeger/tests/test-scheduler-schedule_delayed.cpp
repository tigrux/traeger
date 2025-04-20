// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <future>
#include <traeger/actor/Scheduler.hpp>

TEST_CASE("Scheduler.schedule_delayed")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    auto promise = std::promise<int>{};
    const auto scheduler = Scheduler{Threads{8}};

    const auto start = std::chrono::high_resolution_clock::now();
    scheduler.schedule_delayed(
        10ms,
        [&promise]
        {
            promise.set_value(123);
        });

    REQUIRE(promise.get_future().get() == 123);
    const auto end = std::chrono::high_resolution_clock::now();

    REQUIRE(end - start >= 10ms);
}
