// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include <traeger/actor/Promise.hpp>
#include <traeger/actor/Mailbox.hpp>
#include <traeger/actor/StatelessActor.hpp>

namespace traeger
{
    struct Actor : StatelessActor
    {
        struct Promise : traeger::Promise
        {
            template <typename Callback>
            auto then(Callback &&then_callback) const noexcept -> traeger::Promise
            {
                using CallbackResultType = typename std::invoke_result<Callback, Value>::type;
                return traeger::Promise::then(
                    [then_callback = std::forward<Callback>(then_callback)](const Value &value) -> Result
                    {
                        try
                        {
                            if constexpr (!std::is_same<CallbackResultType, void>::value)
                            {
                                return Result{Value{then_callback(value)}};
                            }
                            else
                            {
                                then_callback(value);
                                return Result{Value{}};
                            }
                        }
                        catch (const std::exception &e)
                        {
                            return Result{Error{e.what()}};
                        }
                    });
            }
        };

        struct Mailbox : traeger::Mailbox
        {
            template <typename... Args>
            auto send(const Scheduler &scheduler,
                      const String &name,
                      Args &&...args) const noexcept -> Promise
            {
                const auto arguments = make_list(std::forward<Args>(args)...);
                return Promise{traeger::Mailbox::send(scheduler, name, arguments)};
            }
        };

        auto mailbox() const noexcept -> Mailbox
        {
            return Mailbox{StatelessActor::mailbox()};
        }
    };

    template <typename State>
    struct StatefulActor : Actor
    {
    public:
        template <typename... Args>
        StatefulActor(Args &&...args) noexcept
            : Actor(),
              state_(std::make_shared<State>(std::forward<Args>(args)...))
        {
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (State::*method)(Args...)) const noexcept -> void
        {
            StatelessActor::define_writer(name, make_function<Return, decltype(method), Args...>(std::move(method)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (State::*method)(Args...) noexcept) const noexcept -> void
        {
            StatelessActor::define_writer(name, make_function<Return, decltype(method), Args...>(std::move(method)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (State::*method)(Args...) const) const noexcept -> void
        {
            StatelessActor::define_reader(name, make_function<Return, decltype(method), Args...>(std::move(method)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (State::*method)(Args...) const noexcept) const noexcept -> void
        {
            StatelessActor::define_reader(name, make_function<Return, decltype(method), Args...>(std::move(method)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (*function)(State &, Args...)) const noexcept -> void
        {
            StatelessActor::define_writer(name, make_function<Return, decltype(function), Args...>(std::move(function)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (*function)(State &, Args...) noexcept) const noexcept -> void
        {
            StatelessActor::define_writer(name, make_function<Return, decltype(function), Args...>(std::move(function)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (*function)(const State &, Args...)) const noexcept -> void
        {
            StatelessActor::define_reader(name, make_function<Return, decltype(function), Args...>(std::move(function)));
        }

        template <typename Return, typename... Args>
        auto define(const String &name,
                    Return (*function)(const State &, Args...) noexcept) const noexcept -> void
        {
            StatelessActor::define_reader(name, make_function<Return, decltype(function), Args...>(std::move(function)));
        }

        template <typename Lambda>
        auto define(const String &name,
                    Lambda &&lambda) const noexcept -> void
        {
            define(name, std::forward<Lambda>(lambda), &Lambda::operator());
        }

    private:
        template <typename Lambda, typename Return, typename... Args>
        auto define(const String &name,
                    Lambda &&lambda,
                    Return (Lambda::*)(State &, Args...) const) const noexcept -> void
        {
            StatelessActor::define_writer(name, make_function<Return, decltype(lambda), Args...>(std::forward<Lambda>(lambda)));
        }

        template <typename Lambda, typename Return, typename... Args>
        auto define(const String &name,
                    Lambda &&lambda,
                    Return (Lambda::*)(State &, Args...)) const noexcept -> void
        {
            StatelessActor::define_writer(name, make_function<Return, decltype(lambda), Args...>(std::forward<Lambda>(lambda)));
        }

        template <typename Lambda, typename Return, typename... Args>
        auto define(const String &name,
                    Lambda &&lambda,
                    Return (Lambda::*)(const State &, Args...) const) const noexcept -> void
        {
            StatelessActor::define_reader(name, make_function<Return, decltype(lambda), Args...>(std::forward<Lambda>(lambda)));
        }

        template <typename Lambda, typename Return, typename... Args>
        auto define(const String &name,
                    Lambda &&lambda,
                    Return (Lambda::*)(const State &, Args...)) const noexcept -> void
        {
            StatelessActor::define_reader(name, make_function<Return, decltype(lambda), Args...>(std::forward<Lambda>(lambda)));
        }

        template <typename Func, typename Tuple, int... index>
        static auto invoke_func(Func &&func,
                                State &state,
                                Tuple &&tuple,
                                std::integer_sequence<int, index...>)
        {
            return std::invoke(std::forward<Func>(func), state, std::get<index>(std::forward<Tuple>(tuple))...);
        }

        template <typename Return, typename Func, typename... Args>
        auto make_function(Func &&func) const noexcept -> Function
        {
            return [state = state_, func = std::forward<Func>(func)](const List &args) mutable noexcept -> Result
            {
                try
                {
                    using Tuple = std::tuple<Args...>;
                    if constexpr (std::is_same<Return, void>::value)
                    {
                        invoke_func<Func, Tuple>(
                            std::forward<Func>(func),
                            *state,
                            args.get_tuple<Args...>(),
                            std::make_integer_sequence<int, sizeof...(Args)>());
                        return Result{Value{}};
                    }
                    else
                    {
                        return Result{Value{
                            invoke_func<Func, Tuple>(
                                std::forward<Func>(func),
                                *state,
                                args.get_tuple<Args...>(),
                                std::make_integer_sequence<int, sizeof...(Args)>())}};
                    }
                }
                catch (const std::exception &e)
                {
                    return Result{Error{e.what()}};
                }
            };
        }

        std::shared_ptr<State> state_;
    };

    template <typename State, typename... Args>
    auto make_actor(Args &&...args) noexcept -> StatefulActor<State>
    {
        return StatefulActor<State>(std::forward<Args>(args)...);
    }
}
