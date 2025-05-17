// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <thread>
#include <zmq.hpp>
#include <traeger/format/Format.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Queue.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Subscriber.hpp>

TEST_CASE("Context.subscriber")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    auto queue = Queue{};
    auto promise = std::promise<void>{};

    {
        const auto context = Context{};
        const auto [subscriber_optional, error] = context.subscriber("ipc://test-subscriber.socket", {"my_topic"});
        REQUIRE(error.empty());
        REQUIRE(subscriber_optional.has_value());

        const auto &subscriber = subscriber_optional.value();
        const auto scheduler = Scheduler{Threads{8}};

        subscriber.listen(
            scheduler,
            [&queue](const String &, const Value &value)
            {
                queue.push(value);
            });
    }

    {
        auto context = zmq::context_t{};
        auto publisher = zmq::socket_t(context, zmq::socket_type::pub);
        publisher.bind("ipc://test-subscriber.socket");
        const auto &format = Format::json();

        std::this_thread::sleep_for(500ms);
        for (int i = 1; i <= 5; ++i)
        {
            publisher.send(zmq::buffer(String{"my_topic"}), zmq::send_flags::sndmore);
            publisher.send(zmq::buffer(format.name()), zmq::send_flags::sndmore);
            publisher.send(zmq::buffer(format.encode(i * 10).first.value()));
            std::this_thread::sleep_for(50ms);
        }
    }

    std::this_thread::sleep_for(50ms);
    auto events = List{};
    queue.pop(events);
    REQUIRE(events == make_list(10, 20, 30, 40, 50));
}
