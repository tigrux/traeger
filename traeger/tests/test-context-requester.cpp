// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <vector>
#include <zmq.hpp>
#include <traeger/format/Format.hpp>
#include <traeger/actor/Result.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Requester.hpp>

TEST_CASE("Context.requester")
{
    using namespace traeger;
    using namespace std::chrono_literals;

    auto promise = std::promise<Result>{};

    {
        const auto context = Context{};
        const auto &format = Format::json();
        const auto [requester_optional, requester_error] = context.requester("ipc://test-requester.socket", format);
        REQUIRE(requester_error.empty());
        REQUIRE(requester_optional.has_value());

        const auto &requester = requester_optional.value();
        const auto mailbox = requester.mailbox();
        const auto scheduler = Scheduler{Threads{8}};
        mailbox
            .send(scheduler, "multiply", make_list(45, 1000))
            .then(
                [&promise](const Value &value) -> Result
                {
                    promise.set_value(Result{value});
                    return Result{};
                })
            .fail(
                [&promise](const Error &error)
                {
                    promise.set_value(Result{Error{error}});
                });
    }

    {
        auto context = zmq::context_t{};
        auto router = zmq::socket_t(context, zmq::socket_type::router);
        router.bind("ipc://test-requester.socket");

        std::vector<std::string> requests;
        while (true)
        {
            zmq::message_t response;
            REQUIRE(router.recv(response, zmq::recv_flags::none).has_value());
            requests.push_back(response.to_string());
            if (!response.more())
            {
                break;
            }
        }
        REQUIRE(requests.size() == 4);

        const auto id = requests.at(0);
        const auto method_name = requests.at(1);
        const auto format_name = requests.at(2);
        const auto request_encoded = requests.at(3);

        REQUIRE(format_name == "json");
        const auto *format = Format::by_name(format_name);
        REQUIRE(format != nullptr);
        const auto arguments = format->decode(request_encoded)
                                   .first
                                   .value()
                                   .get_list()
                                   .value();

        auto result = Result{};
        if (method_name == "multiply")
        {
            Int a = 0, b = 0;
            if (auto [ok, error] = arguments.unpack(a, b); !ok)
            {
                result = Result{Error{error}};
            }
            else
            {
                result = Result{Value{a * b}};
            }
        }
        else
        {
            result = Result{Error{"no such method " + method_name}};
        }

        router.send(zmq::buffer(id), zmq::send_flags::sndmore);
        REQUIRE(result.type() != Result::Type::Undefined);
        if (result.value())
        {
            const auto value_encoded = format->encode(*result.value()).first.value();
            router.send(zmq::buffer(value_encoded), zmq::send_flags::sndmore);
            router.send(zmq::buffer(std::string{}), zmq::send_flags::none);
        }
        if (result.error())
        {
            router.send(zmq::buffer(std::string{}), zmq::send_flags::sndmore);
            router.send(zmq::buffer(*result.error()), zmq::send_flags::none);
        }
    }

    auto response_result = promise.get_future().get();
    REQUIRE(response_result == Result{Value{45000}});
}
