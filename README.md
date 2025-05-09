# Traeger: A portable Actor System for C++ and Python
Traeger is a collection of libraries to write applications following the principles of the [Actor Model](https://en.wikipedia.org/wiki/Actor_model).

* It simplifies the development of concurrent, distributed and portable applications.
* It is writen in a subset of C++ 17 that works on multiple platforms (Linux, MacOS, Windows) and architectures (x86_64 and arm64).
* Its libraries do not have dependencies, aside from the standard C++ runtime.
* It does not reinvent the wheel and instead leverages battle-tested projects like
  [immer](https://github.com/arximboldi/immer/),
  [json](https://github.com/nlohmann/json),
  [yaml-cpp](https://github.com/jbeder/yaml-cpp/),
  [msgpack](https://github.com/msgpack/msgpack-c/),
  [zeromq](https://github.com/zeromq/libzmq), and
  [nanobind](https://github.com/wjakob/nanobind).
* Bindings for a few other languages are provided:
    * Python: for prototyping and testing.
    * Go: for networking and micro-services.
    * C: for interfacing with other languages via [FFI](https://doc.rust-lang.org/nomicon/ffi.html).
* The word Traeger is German for carrier and the name was inspired by the library [immer](https://github.com/arximboldi/immer/).

# Prerequisites
* cmake >= 3.24:
    * On MacOS it can be installed using [brew](https://brew.sh).
    * On Windows it is recommended to use the developer terminal of [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
    * On Linux it is recommended to install it from the [official project page](https://cmake.org/download/).
* A compiler with good support of C++ 17: clang >= 6.0, or g++ >= 7.5.

Traeger has some dependencies that are downloaded by cmake as part of the build process via [FetchContent](https://cmake.org/cmake/help/v3.24/module/FetchContent.html).
The dependencies are compiled statically to avoid dependencies in the resulting libraries.

# Installation
The installation is the typical for a cmake based project.
In the following instructions we are assuming that Traeger is going to be installed on the directory `${HOME}/local`.

```shell
$ git clone https://github.com/tigrux/traeger traeger
$ cd traeger
$ cmake -B tmp_builddir
$ cmake --build tmp_builddir --parallel
$ cmake --install tmp_builddir --prefix ${HOME}/local
```

Tests are run with cmake as well:

```shell
$ cmake --build tmp_builddir --target test
```

The python bindings are installed via pip:

```shell
$ CMAKE_PREFIX_PATH=${HOME}/local pip install ./python/
```

# Quick start
The following examples, as well as examples for [Python](./python/examples) and [Go](./go/examples), are available in the directory [examples](./examples).

## Value semantics
Traeger provides a library `traeger::value` with types that can be efficiently copied and shared without data-races.

`Value` is a tagged union of the following types:
* `Null` represents `null` i.e. an empty value. This is the default type.
* `Bool` represents either `false` or `true`.
* `Int` represents a signed 64-bit integer.
* `UInt` represents an unsigned 64-bit integer that cannot be represented as a signed integer.
* `Float` represents a 64-bit double precision float number.
* `String` represents a string of characters.
* `List` represents a sequence of values.
* `Map` represents a key value mapping using strings as keys.

```C++
// FILE: examples/example-value-semantics.cpp
// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <traeger/value/Value.hpp>

int main()
{
    auto list1 = traeger::make_list(2, 3, 5);
    std::cout << "This is list1: " << list1 << std::endl;

    auto list2 = list1;
    list2.set(0, 1);
    list2.append(7);
    std::cout << "The list2 has odd numbers: " << list2 << std::endl;

    auto list3 = list1;
    list3.append(7);
    std::cout << "The list3 has prime numbers: " << list3 << std::endl;

    auto map1 = traeger::make_map(
        "odds", list2,
        "primes", list3);
    std::cout << "A map with the numbers:" << std::endl;
    for (const auto [key, value] : map1)
    {
        std::cout << key << ": " << value << std::endl;
    }
}
```

## Actors
Traeger provides a library `traeger::actor` with abstractions for concurrency and parallelism.

### Defining actors
* Actors are the basic building block of concurrency.
* Actors keep a private state whose type is specified as a class.
* The methods of the class are used to define the messages that an actor can receive.
    * Methods not marked as `const` define writers i.e. methods that can modify the state of the actor.
    * Methods marked as `const` define readers  i.e. methods that can not modify the state of the actor.
* The user of the actor only needs to know the messages that an actor can receive.
* Actors can be created in functions that act as factories, hiding any implementation details.

```C++
// FILE: examples/example-actor-definition.cpp
// SPDX-License-Identifier: BSL-1.0

#include <thread>
#include <chrono>

#include <traeger/actor/Actor.hpp>

class Account
{
    traeger::Float funds_;

public:
    Account(traeger::Float initial_funds) noexcept
        : funds_(initial_funds)
    {
    }

    // this method is non-const i.e. it modifies the instance
    auto deposit(traeger::Float amount) -> traeger::Float
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
    auto debit(traeger::Float amount) -> traeger::Float
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

// this method acts as the factory for the Account actor
traeger::Actor make_account_actor(traeger::Float initial_funds)
{
    auto account_actor = traeger::make_actor<Account>(initial_funds);
    account_actor.define("deposit", &Account::deposit);
    account_actor.define("debit", &Account::debit);
    account_actor.define("balance", &Account::balance);
    return std::move(account_actor);
}
```

### Sending messages
* Actors react by sending messages to their mailbox.
    * Messages associated to writers are executed sequentially in the same order they were received.
    * Messages associated to readers are executed concurrently as long as no writer is currently in execution.
* A Scheduler is used to execute the messages asynchronously.
* Upon sending a message a promise is returned.
* Promises provide methods to call your code back with either:
    * The value returned by the message.
    * The error of the exception that was thrown.

```C++
// FILE: examples/example-actor-messaging.cpp
// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <thread>
#include <chrono>

#include <traeger/actor/Actor.hpp>

void perform_operations(traeger::Scheduler scheduler, traeger::Mailbox mailbox)
{
    // Use a decorator to enable type deduction in C++
    auto actor_mailbox = traeger::Actor::Mailbox{mailbox};

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
```

### The scheduler
* A scheduler maintains a pool of threads that are used to execute messages.
* The scheduler counts the number of messages pending or in execution.
* Applications may want to wait until the scheduler count reaches zero, otherwise messages may not complete.

```C++
// FILE: examples/example-actor-local.cpp
// SPDX-License-Identifier: BSL-1.0

#include <thread>
#include <chrono>

#include <traeger/actor/Actor.hpp>

extern traeger::Actor make_account_actor(traeger::Float initial_funds);

extern void perform_operations(traeger::Scheduler scheduler, traeger::Mailbox mailbox);

int main()
{
    auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    auto account_actor = make_account_actor(0.0);

    perform_operations(scheduler, account_actor.mailbox());

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

## Serialization
Traeger provides a library `traeger::format` with encoders and decoders for common formats like [JSON](https://en.wikipedia.org/wiki/JSON), [YAML](https://en.wikipedia.org/wiki/YAML), and [MessagePack](https://en.wikipedia.org/wiki/MessagePack).
* [JSON](https://github.com/nlohmann/json) is useful to exchange messages with a format that is human-readable.
* [MessagePack](https://github.com/msgpack/msgpack) is similar in functionality to JSON, but using a binary format that is more compact and efficient.
* [YAML](https://github.com/jbeder/yaml-cpp) is better to format configuration files.

```C++
// FILE: examples/example-format-serialization.cpp
// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>

int main()
{
    auto value = traeger::Value{
        traeger::make_map(
            "name", "John",
            "age", 30,
            "married", true)};
    std::cout << "The original value is " << value << std::endl;

    const auto &json = traeger::Format::json();
    auto [encode_optional, encode_error] = json.encode(value);
    if (!encode_optional)
    {
        std::cerr << "Encode error " << encode_error << std::endl;
        return 1;
    }

    const auto encoded_string = encode_optional.value();
    std::cout << "The encoded json is " << encoded_string << std::endl;

    auto [decode_optional, decode_error] = json.decode(encoded_string);
    if (!decode_optional)
    {
        std::cerr << "Decode error " << decode_error << std::endl;
        return 1;
    }

    const auto decoded_value = decode_optional.value();
    std::cout << "The decoded value is " << decoded_value << std::endl;
}
```

## Networking
Traeger provides a library `traeger::socket` with abstractions to perform [operations over a network using common patterns](https://zguide.zeromq.org/docs/chapter2/).

### The request/reply pattern
In this pattern a replier binds a mailbox to an address to which requesters connect to send it messages.

#### The replier
* A replier is used to expose the mailbox of an actor over a network.
* The replier binds a socket to a network address.
* Data received by the socket is decoded then redirected to the mailbox.
* As the messages complete the socket replies back with the encoded results.

```C++
// FILE: examples/example-socket-replier.cpp
// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <iostream>
#include <thread>
#include <string>

#include <traeger/actor/Actor.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Replier.hpp>

extern traeger::Actor make_account_actor(traeger::Float initial_funds);

int main()
{
    auto context = traeger::Context{};
    const char *address = "tcp://*:5555";

    auto [replier_optional, replier_error] = context.replier(address);
    if (!replier_optional)
    {
        std::cerr << "Socket error = " << replier_error << std::endl;
        return 1;
    }
    auto replier = replier_optional.value();

    auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    auto actor = make_account_actor(0.0);
    auto reply_promise = replier.reply(scheduler, actor.mailbox());
    std::cout << "Replier listening on address: " << address << std::endl;

    std::string dummy_string;
    std::cout << "Enter a new line feed to stop" << std::endl;
    std::getline(std::cin, dummy_string);
    reply_promise.set_result(traeger::Value{nullptr});
}
```

#### The requester
* A requester is used to send messages to a replier.
* A requester connects a socket to the network address of a replier.
* Data received by the socket is encoded then send over to the replier.
* The requester decodes responses and use them to complete messages.

 ```C++
// FILE: examples/example-socket-requester.cpp
// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <iostream>
#include <thread>

#include <traeger/format/Format.hpp>
#include <traeger/actor/Actor.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Requester.hpp>

extern void perform_operations(traeger::Scheduler scheduler, traeger::Mailbox mailbox);

int main()
{
    auto context = traeger::Context{};
    const char *address = "tcp://localhost:5555";
    const auto &format = traeger::Format::json();

    auto [requester_optional, requester_error] = context.requester(address, format);
    if (!requester_optional)
    {
        std::cerr << "Socket error = " << requester_error << std::endl;
        return 1;
    }
    auto requester = requester_optional.value();

    auto scheduler = traeger::Scheduler{traeger::Threads{8}};

    std::cout << "Sending messages to replier on address: " << address << std::endl;
    perform_operations(scheduler, requester.mailbox());

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

### The publisher/subscriber pattern
In this pattern a publisher binds to an address to multicast values grouped by topic, subscribers listen to values per topic.

#### The publisher
* A publisher is used to multicast values over a network grouped by topic.
* A publisher multicasts regardless if there are subscribers or not.

 ```C++
// FILE: examples/example-socket-publisher.cpp
// SPDX-License-Identifier: BSL-1.0

#include <chrono>
#include <iostream>
#include <thread>

#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>
#include <traeger/actor/Scheduler.hpp>
#include <traeger/socket/Context.hpp>
#include <traeger/socket/Publisher.hpp>

void heart_beat(traeger::Scheduler scheduler, traeger::Publisher publisher, traeger::Int counter)
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
    auto context = traeger::Context{};
    const char *address = "tcp://*:5556";
    const auto &format = traeger::Format::json();

    auto [publisher_optional, publisher_error] = context.publisher(address, format);
    if (!publisher_optional)
    {
        std::cerr << "Socket bind error: " << publisher_error << std::endl;
        return 1;
    }
    auto publisher = publisher_optional.value();

    auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    std::cout << "Publishing heart-beat events on address: " << address << std::endl;
    heart_beat(scheduler, publisher, 0);

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

#### The subscriber
* A subscriber connects to the address of a publisher.
* A subscriber listens for values ​​of topics to which it has subscribed.

 ```C++
// FILE: examples/example-socket-subscriber.cpp
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
    auto context = traeger::Context{};
    const char *address = "tcp://localhost:5556";
    auto topics = std::vector<traeger::String>{"heart-beat"};

    auto [subscriber_optional, subscriber_error] = context.subscriber(address, topics);
    if (!subscriber_optional)
    {
        std::cerr << "Socket bind error: " << subscriber_error << std::endl;
        return 1;
    }
    auto subscriber = subscriber_optional.value();

    auto scheduler = traeger::Scheduler{traeger::Threads{8}};
    auto last_heart_beat = std::chrono::system_clock::now();

    std::cout << "Listening for heart-beat events on address: " << address << std::endl;
    subscriber.listen(
        scheduler,
        [&last_heart_beat](traeger::String topic, traeger::Value value)
        {
            last_heart_beat = std::chrono::system_clock::now();
            std::cout << topic << ": " << value << std::endl;
        });

    while (scheduler.count() != 0)
    {
        auto now = std::chrono::system_clock::now();
        if (now - last_heart_beat >= std::chrono::seconds(2))
        {
            std::cout << "The last heart-beat was more than 2s ago" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

# License
Boost Software License - Version 1.0
