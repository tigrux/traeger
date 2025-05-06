// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Mailbox.hpp>

namespace
{
    using namespace traeger;

    struct AdderMailbox : public Mailbox::Interface
    {
        Promise
        send(const Scheduler &scheduler,
             const String &name,
             const List &arguments) noexcept override
        {
            auto promise = Promise{scheduler};
            if (name == "add")
            {
                scheduler.schedule(
                    [promise, arguments]
                    {
                        Int sum = 0;
                        for (const auto &value : arguments)
                        {
                            if (auto arg_int = value.get_int(); arg_int)
                            {
                                sum += *arg_int;
                            }
                        }
                        promise.set_result(Value{sum});
                    });
            }
            else
            {
                promise.set_result(Error{"Not a method"});
            }

            return promise;
        }
    };
}

TEST_CASE("Mailbox.send")
{
    using namespace traeger;

    auto scheduler = Scheduler{Threads{8}};
    auto adder_mailbox = AdderMailbox{};

    SECTION("value")
    {
        auto promise_value = std::promise<Value>{};
        adder_mailbox
            .send(scheduler, "add", make_list(100, 20, 3))
            .then(
                [&promise_value](const Value &value) -> Result
                {
                    promise_value.set_value(value);
                    return Result{};
                });

        REQUIRE(promise_value.get_future().get() == 123);
    }

    SECTION("error")
    {
        auto promise_error = std::promise<Error>{};
        adder_mailbox
            .send(scheduler, "mul", make_list(2, 3, 5))
            .fail(
                [&promise_error](const Error &error) -> void
                {
                    promise_error.set_value(error);
                });

        REQUIRE(promise_error.get_future().get() == Error{"Not a method"});
    }
}