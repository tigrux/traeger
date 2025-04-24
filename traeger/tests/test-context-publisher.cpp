// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <vector>
#include <thread>
#include <zmq.hpp>
#include <traeger/format/Format.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Publisher.hpp>

TEST_CASE("Context.publisher")
{
    using namespace std::chrono_literals;
    using namespace traeger;

    {
        const auto context = Context{};
        const auto &format = Format::json();
        const auto [publisher_optional, error] = context.publisher("ipc://test-publisher.socket", format);
        REQUIRE(error == "");
        REQUIRE(publisher_optional.has_value());

        const auto &publisher = publisher_optional.value();
        const auto scheduler = Scheduler{Threads{8}};

        scheduler.schedule_delayed(
            500ms,
            [scheduler, publisher]
            {
                for (int i = 1; i <= 5; ++i)
                {
                    publisher.publish(scheduler, "my_topic", i * 10);
                }
            });
    }

    auto events = List{};
    {
        auto context = zmq::context_t{};
        auto subscriber = zmq::socket_t(context, zmq::socket_type::sub);
        subscriber.connect("ipc://test-publisher.socket");
        subscriber.set(zmq::sockopt::subscribe, "my_topic");

        for (int i = 1; i <= 5; ++i)
        {
            std::vector<std::string> responses;
            while (true)
            {
                zmq::message_t response;
                REQUIRE(subscriber.recv(response, zmq::recv_flags::none));
                responses.push_back(response.to_string());
                if (!response.more())
                {
                    break;
                }
            }

            REQUIRE(responses.size() == 3);
            const auto topic_name = responses.at(0);
            const auto format_name = responses.at(1);
            const auto value_encoded = responses.at(2);

            REQUIRE(topic_name == "my_topic");
            const auto *format = Format::by_name(format_name);
            const auto value = format->decode(value_encoded).first.value();
            events.append(value);
        }
    }

    REQUIRE(events == make_list(10, 20, 30, 40, 50));
}
