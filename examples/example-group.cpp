#include <chrono>
#include <iostream>
#include <map>
#include <optional>
#include <string_view>
#include <thread>
#include <traeger/actor/Actor.hpp>
#include <traeger/actor/Group.hpp>
#include <traeger/actor/Queue.hpp>

struct Division
{
    auto div(traeger::Float a, traeger::Float b) const -> traeger::Float
    {
        if (b == 0.0)
        {
            throw std::runtime_error("division by zero");
        }
        return a / b;
    }

    static auto make() noexcept -> traeger::Mailbox
    {
        auto actor = traeger::make_actor<Division>();
        actor.define("div", &Division::div);
        return std::move(actor.mailbox());
    }
};

int main()
{
    auto scheduler = traeger::Scheduler{traeger::Threads{8}};

    auto arithmetic = traeger::Group{};
    arithmetic.add("Division", Division::make());
    auto math = traeger::Group{};
    math.add("Arithmetic", arithmetic.mailbox());

    auto queue = traeger::Queue{};
    auto mailbox = math.mailbox();
    for (auto divisor : {25, 10, 5, 0})
    {
        mailbox
            .send(scheduler, "Arithmetic/Division/div", traeger::make_list(100, divisor))
            .then(
                [&queue](const traeger::Value &map_value) -> traeger::Result
                {
                    queue.push(map_value);
                    return traeger::Result{};
                })
            .fail(
                [&queue](const traeger::Error &error)
                {
                    queue.push(make_map("error", error));
                });
    }
    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    queue.close();

    auto values = traeger::List{};
    queue.pop(values);
    for (const auto &value : values)
    {
        std::cout << value << std::endl;
    }
}