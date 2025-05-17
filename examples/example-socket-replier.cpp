// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <string>

#include <traeger/actor/Actor.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Replier.hpp>

extern traeger::Actor make_account_actor(traeger::Float initial_funds);

int main()
{
    const auto context = traeger::Context{};
    const auto *address = "tcp://*:5555";

    auto [replier_optional, replier_error] = context.replier(address);
    if (!replier_optional)
    {
        std::cerr << "Socket error = " << replier_error << std::endl;
        return 1;
    }
    const auto replier = replier_optional.value();

    const auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    const auto actor = make_account_actor(0.0);
    const auto reply_promise = replier.reply(scheduler, actor.mailbox());
    std::cout << "Replier listening on address: " << address << std::endl;

    std::string dummy_string;
    std::cout << "Enter a new line feed to stop" << std::endl;
    std::getline(std::cin, dummy_string);
    reply_promise.set_result(traeger::Result{traeger::Value{nullptr}});
}
