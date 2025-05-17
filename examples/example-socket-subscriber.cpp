// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <iostream>
#include <thread>

#include <traeger/value/Value.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Subscriber.hpp>

int main()
{
    const auto context = traeger::Context{};
    const auto *address = "tcp://localhost:5556";
    const auto topics = std::vector<traeger::String>{"heart-beat"};

    auto [subscriber_optional, subscriber_error] = context.subscriber(address, topics);
    if (!subscriber_optional)
    {
        std::cerr << "Socket bind error: " << subscriber_error << std::endl;
        return 1;
    }
    const auto subscriber = subscriber_optional.value();

    const auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    auto last_heart_beat = std::chrono::system_clock::now();

    std::cout << "Listening for heart-beat events on address: " << address << std::endl;
    subscriber.listen(
        scheduler,
        [&last_heart_beat](const traeger::String &topic, const traeger::Value &value)
        {
            last_heart_beat = std::chrono::system_clock::now();
            std::cout << topic << ": " << value << std::endl;
        });

    while (scheduler.count() != 0)
    {
        if (auto now = std::chrono::system_clock::now();
            now - last_heart_beat >= std::chrono::seconds(2))
        {
            std::cout << "The last heart-beat was more than 2s ago" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
