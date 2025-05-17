// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <iostream>
#include <thread>

#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Publisher.hpp>

void heart_beat(const traeger::Scheduler &scheduler, const traeger::Publisher &publisher, const traeger::Int counter)
{
    scheduler.schedule_delayed(
        std::chrono::seconds(1),
        [scheduler, publisher, counter]
        {
            std::cout << "Broadcasting heart-beat: " << counter << std::endl;
            publisher.publish(scheduler, "heart-beat", counter);
            if (counter < 10)
            {
                heart_beat(scheduler, publisher, counter + 1);
            }
        });
}

int main()
{
    const auto context = traeger::Context{};
    const auto *address = "tcp://*:5556";
    const auto &format = traeger::Format::json();

    auto [publisher_optional, publisher_error] = context.publisher(address, format);
    if (!publisher_optional)
    {
        std::cerr << "Socket bind error: " << publisher_error << std::endl;
        return 1;
    }
    const auto publisher = publisher_optional.value();

    const auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    std::cout << "Publishing heart-beat events on address: " << address << std::endl;
    heart_beat(scheduler, publisher, 0);

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
