// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Queue.hpp>

TEST_CASE("Queue.count")
{
    using namespace traeger;

    auto queue = Queue{};

    REQUIRE(queue.count() == 0);
    queue.push(10);
    REQUIRE(queue.count() == 1);
    queue.push(20);
    REQUIRE(queue.count() == 2);
    queue.push(30);
    REQUIRE(queue.count() == 3);
}
