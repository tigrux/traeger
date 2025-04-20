// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Queue.hpp>

TEST_CASE("Queue.close")
{
    using namespace traeger;

    auto queue = Queue{};
    REQUIRE_FALSE(queue.closed());

    REQUIRE(queue.push(10));
    REQUIRE(queue.push(20));
    REQUIRE(queue.push(30));

    queue.close();
    REQUIRE(queue.closed());
    REQUIRE_FALSE(queue.push(40));

    REQUIRE(queue.count() == 3);
    auto list = List{};
    queue.pop(list);
    REQUIRE(Value{list} == make_list(10, 20, 30));
}
