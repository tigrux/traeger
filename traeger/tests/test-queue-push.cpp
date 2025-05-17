// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Queue.hpp>

TEST_CASE("Queue.push")
{
    using namespace traeger;

    auto queue = Queue{};
    const auto scheduler = Scheduler{Threads{8}};
    scheduler.schedule(
        [&queue]
        {
            queue.push(10);
            queue.push(20);
            queue.push(30);
            queue.close();
        });

    auto list = List{};
    while (auto optional = queue.pop())
    {
        list.append(optional.value());
    }
    REQUIRE(list == make_list(10, 20, 30));
}
