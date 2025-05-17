// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <thread>
#include <chrono>

#include <traeger/actor/Actor.hpp>

void perform_operations(const traeger::Scheduler &scheduler, const traeger::Mailbox &mailbox)
{
    // Use a decorator to enable type deduction in C++
    const auto actor_mailbox = traeger::Actor::Mailbox{mailbox};

    for (auto [operation, amount] : {
             std::make_pair("deposit", 1000),
             std::make_pair("deposit", 500),
             std::make_pair("deposit", 0),
             std::make_pair("debit", -2000),
             std::make_pair("debit", 750),
             std::make_pair("deposit", 250),
             std::make_pair("debit", 500),
         })
    {
        std::cout << "Performing " << operation << " " << amount << std::endl;
        actor_mailbox
            .send(scheduler, operation, amount)
            .then(
                [operation = operation](const traeger::Value &balance_value)
                {
                    std::cout << "Balance after " << operation
                              << " is " << balance_value << std::endl;
                })
            .fail(
                [operation = operation](const traeger::Error &operation_error)
                {
                    std::cout << "Error performing " << operation
                              << ": " << operation_error << std::endl;
                });
    }

    actor_mailbox
        .send(scheduler, "balance")
        .then(
            [](const traeger::Value &balance_value)
            {
                std::cout << "The balance is " << balance_value << std::endl;
            });
}
