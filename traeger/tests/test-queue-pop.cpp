// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <chrono>
#include <iostream>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Queue.hpp>

TEST_CASE("Queue.pop")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    auto queue = Queue{};
    auto scheduler = Scheduler{Threads{8}};

    auto promise = std::promise<void>{};
    scheduler.schedule(
        [&queue, &promise]
        {
            for (int i = 1; i <= 4; ++i)
            {
                queue.push(i * 10);
            }
            promise.set_value();
            for (int i = 5; i <= 8; ++i)
            {
                queue.push(i * 10);
            }
            queue.close();
        });

    promise.get_future().get();
    auto list = List{};
    REQUIRE(queue.count() >= 4);

    queue.pop(list);
    REQUIRE(list.size() >= 4);

    while (auto optional_value = queue.pop())
    {
        list.append(optional_value.value());
    }
    REQUIRE(list == make_list(10, 20, 30, 40, 50, 60, 70, 80));
}
