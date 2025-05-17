// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <iostream>
#include <thread>

#include <traeger/format/Format.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Requester.hpp>

extern void perform_operations(const traeger::Scheduler &scheduler, const traeger::Mailbox &mailbox);

int main()
{
    const auto context = traeger::Context{};
    const auto *address = "tcp://localhost:5555";
    const auto &format = traeger::Format::json();

    auto [requester_optional, requester_error] = context.requester(address, format);
    if (!requester_optional)
    {
        std::cerr << "Socket error = " << requester_error << std::endl;
        return 1;
    }
    const auto requester = requester_optional.value();

    const auto scheduler = traeger::Scheduler{traeger::Threads{8}};

    std::cout << "Sending messages to replier on address: " << address << std::endl;
    perform_operations(scheduler, requester.mailbox());

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
