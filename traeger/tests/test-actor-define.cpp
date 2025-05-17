// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/actor/Actor.hpp>
#include <utility>

namespace
{
    class User
    {
        std::string name_;

    public:
        explicit User(std::string name)
            : name_(std::move(name))
        {
        }

        std::string get_name() const
        {
            return name_;
        }

        void set_name(std::string name)
        {
            name_ = std::move(name);
        }
    };

    std::string user_get_name(const User &user)
    {
        return user.get_name();
    }

    void user_set_name(User &user, std::string name)
    {
        return user.set_name(std::move(name));
    }
}

TEST_CASE("Actor.define")
{
    using namespace traeger;

    const auto scheduler = Scheduler{Threads{8}};
    const auto actor = make_actor<User>("John");

    SECTION("methods")
    {
        actor.define("get_name", &User::get_name);
        actor.define("set_name", &User::set_name);
        const auto mailbox = actor.mailbox();

        SECTION("read")
        {
            auto promise = std::promise<Value>{};
            mailbox.send(scheduler, "get_name")
                .then(
                    [&promise](const Value &value)
                    {
                        promise.set_value(value);
                    });
            REQUIRE(promise.get_future().get() == "John");
        }

        SECTION("write")
        {
            auto promise = std::promise<Value>{};
            mailbox.send(scheduler, "set_name", "Jack");
            mailbox
                .send(scheduler, "get_name")
                .then(
                    [&promise](const Value &value)
                    {
                        promise.set_value(value);
                    });
            REQUIRE(promise.get_future().get() == "Jack");
        }

        SECTION("error-no-such-method")
        {
            auto promise = std::promise<Error>{};
            mailbox
                .send(scheduler, "set_no_name")
                .fail(
                    [&promise](const Error &error)
                    {
                        promise.set_value(error);
                    });
            REQUIRE(promise.get_future().get() == "no such actor method set_no_name");
        }

        SECTION("error-invalid-cast")
        {
            auto promise = std::promise<Error>{};
            mailbox
                .send(scheduler, "set_name", 123456)
                .fail(
                    [&promise](const Error &error)
                    {
                        promise.set_value(error);
                    });
            REQUIRE(promise.get_future().get() == "invalid cast in argument 0 from type Int to String");
        }

        SECTION("error-expected-arguments")
        {
            auto promise = std::promise<Error>{};
            mailbox
                .send(scheduler, "set_name")
                .fail(
                    [&promise](const Error &error)
                    {
                        promise.set_value(error);
                    });
            REQUIRE(promise.get_future().get() == "expected 1 arguments but 0 were given");
        }
    }

    SECTION("functions")
    {
        actor.define("get_name", &user_get_name);
        actor.define("set_name", &user_set_name);
        auto mailbox = actor.mailbox();

        SECTION("read")
        {
            auto promise = std::promise<Value>{};
            mailbox.send(scheduler, "get_name")
                .then(
                    [&promise](const Value &value)
                    {
                        promise.set_value(value);
                    });
            REQUIRE(promise.get_future().get() == "John");
        }

        SECTION("write")
        {
            auto promise = std::promise<Value>{};
            mailbox.send(scheduler, "set_name", "Joann");
            mailbox
                .send(scheduler, "get_name")
                .then(
                    [&promise](const Value &value)
                    {
                        promise.set_value(value);
                    });
            REQUIRE(promise.get_future().get() == "Joann");
        }
    }

    SECTION("lambdas")
    {
        actor.define("get_name",
                     [](const User &user)
                     { return user.get_name(); });
        actor.define("set_name",
                     [](User &user, String name)
                     { user.set_name(std::move(name)); });
        auto mailbox = actor.mailbox();

        SECTION("read")
        {
            auto promise = std::promise<Value>{};
            mailbox.send(scheduler, "get_name")
                .then(
                    [&promise](const Value &value)
                    {
                        promise.set_value(value);
                    });
            REQUIRE(promise.get_future().get() == "John");
        }

        SECTION("write")
        {
            auto promise = std::promise<Value>{};
            mailbox.send(scheduler, "set_name", "Jack");
            mailbox
                .send(scheduler, "get_name")
                .then(
                    [&promise](const Value &value)
                    {
                        promise.set_value(value);
                    });
            REQUIRE(promise.get_future().get() == "Jack");
        }
    }
}