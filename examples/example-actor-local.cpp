// SPDX-License-Identifier: BSL-1.0

#include <thread>
#include <chrono>

#include <traeger/actor/Actor.hpp>

extern traeger::Actor make_account_actor(traeger::Float initial_funds);

extern void perform_operations(const traeger::Scheduler &scheduler, const traeger::Mailbox &mailbox);

int main()
{
    const auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    const auto account_actor = make_account_actor(0.0);

    perform_operations(scheduler, account_actor.mailbox());

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
