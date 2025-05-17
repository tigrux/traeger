// SPDX-License-Identifier: BSL-1.0

#include <thread>
#include <chrono>

#include <traeger/actor/Actor.hpp>

class Account
{
    traeger::Float funds_;

public:
    explicit Account(const traeger::Float initial_funds) noexcept
        : funds_(initial_funds)
    {
    }

    // this method is non-const i.e. it modifies the instance
    auto deposit(const traeger::Float amount) -> traeger::Float
    {
        // simulate a costly operation
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (amount <= 0)
        {
            throw std::runtime_error("invalid amount");
        }
        funds_ += amount;
        return funds_;
    }

    // this method is non-const i.e. it modifies the instance
    auto debit(const traeger::Float amount) -> traeger::Float
    {
        // simulate a costly operation
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (amount <= 0)
        {
            throw std::runtime_error("invalid amount");
        }
        if (funds_ < amount)
        {
            throw std::runtime_error("not enough funds");
        }
        funds_ -= amount;
        return funds_;
    }

    // this method is const i.e. it does not modify the instance
    auto balance() const noexcept -> traeger::Float
    {
        // simulate a costly operation
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return funds_;
    }
};

// this function acts as the factory for the Account actor
traeger::Actor make_account_actor(traeger::Float initial_funds)
{
    auto account_actor = traeger::make_actor<Account>(initial_funds);
    account_actor.define("deposit", &Account::deposit);
    account_actor.define("debit", &Account::debit);
    account_actor.define("balance", &Account::balance);
    return static_cast<traeger::Actor>(std::move(account_actor));
}
