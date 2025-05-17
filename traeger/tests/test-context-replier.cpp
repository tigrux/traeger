// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <zmq.hpp>
#include <traeger/format/Format.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/StatelessActor.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Replier.hpp>

TEST_CASE("Context.replier")
{
    using namespace traeger;

    {
        const auto actor = StatelessActor{};
        actor.define_reader(
            "multiply",
            [](const List &arguments) -> Result
            {
                Int a = 0, b = 0;
                arguments.unpack(a, b);
                return Result{Value{a * b}};
            });

        const auto context = Context{};
        const auto [replier_optional, error] = context.replier("ipc://test-replier.socket");
        REQUIRE(error.empty());
        REQUIRE(replier_optional.has_value());

        const auto &replier = replier_optional.value();
        const auto scheduler = Scheduler{Threads{8}};
        const auto mailbox = actor.mailbox();
        replier.reply(scheduler, mailbox);
    }

    {
        const auto method_name = String{"multiply"};
        const auto &format = Format::json();
        const auto arguments = make_list(45, 1000);

        auto context = zmq::context_t{};
        auto dealer = zmq::socket_t(context, zmq::socket_type::dealer);
        dealer.connect("ipc://test-replier.socket");
        dealer.send(zmq::buffer(method_name), zmq::send_flags::sndmore);
        dealer.send(zmq::buffer(format.name()), zmq::send_flags::sndmore);
        dealer.send(zmq::buffer(format.encode(arguments).first.value()), zmq::send_flags::none);

        std::vector<std::string> responses;
        while (true)
        {
            zmq::message_t response;
            REQUIRE(dealer.recv(response, zmq::recv_flags::none).has_value());
            responses.push_back(response.to_string());
            if (!response.more())
            {
                break;
            }
        }
        REQUIRE(responses.size() == 2);
        auto response_encoded = responses.at(0);
        auto response_error = responses.at(1);
        REQUIRE(response_error.empty());

        auto response_value = format.decode(response_encoded).first.value();
        REQUIRE(response_value == 45000);
    }
}
