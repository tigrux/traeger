# Traeger: A portable Actor System for C++ and other languages
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
    * C: for interfacing with other languages like Rust and Go.
    * Go: for networking and micro-services.
    * Rust: to gain access to its arsenal of crates.
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

This quickstart is available with examples for:
* [C++](/README.md#quick-start).
* [Python](/python/README.md#quick-start).
* [Go](/go/README.md#quick-start).
* [Rust](/rust/README.md#quick-start).


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

```Python
# FILE: python/examples/example-value-semantics.py
# SPDX-License-Identifier: BSL-1.0

import traeger


def main():
    list1: traeger.List = traeger.List([2, 3, 5])
    print(f"This is list1: {list1}")

    list2: traeger.List = list1.copy()
    list2[0] = 1
    list2.append(7)
    print(f"The list2 has odd numbers: {list2}")

    list3: traeger.List = list1.copy()
    list3.append(7)
    print(f"The list2 has prime numbers: {list3}")

    map1: traeger.Map = traeger.Map(
        {
            "odds": list2,
            "primes": list3,
        }
    )
    print(f"A map with the numbers:")
    for key, value in map1.items():
        print(f"{key}: {value}")


if __name__ == "__main__":
    main()
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

```Python
# FILE: python/examples/example_actor_definition.py
# SPDX-License-Identifier: BSL-1.0

import time
import traeger


class Account(traeger.Actor):
    def __init__(self, initial_funds):
        self.funds = initial_funds

    def deposit(self, amount: float) -> float:
        # simulate a costly operation
        time.sleep(0.200)
        if amount <= 0:
            raise RuntimeError("invalid amount")
        self.funds += amount
        return self.funds

    def debit(self, amount: float) -> float:
        # simulate a costly operation
        time.sleep(0.200)
        if amount <= 0:
            raise RuntimeError("invalid amount")
        if self.funds < amount:
            raise RuntimeError("not enough funds")
        self.funds -= amount
        return self.funds

    def balance(self) -> float:
        # simulate a costly operation
        time.sleep(0.200)
        return self.funds


# this function acts as the factory for the Account actor
def make_account_actor(initial_funds: float) -> traeger.Actor:
    account_actor = traeger.make_actor(Account, initial_funds)
    account_actor.define_writer("deposit", Account.deposit)
    account_actor.define_writer("debit", Account.debit)
    account_actor.define_reader("balance", Account.balance)
    return account_actor
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

```Python
# FILE: python/examples/example_actor_messaging.py
# SPDX-License-Identifier: BSL-1.0

import traeger


def perform_operations(scheduler: traeger.Scheduler, mailbox: traeger.Mailbox) -> None:
    for operation, amount in [
        ("deposit", 1000),
        ("deposit", 500),
        ("deposit", 0),
        ("debit", -2000),
        ("debit", 750),
        ("deposit", 250),
        ("debit", 500),
    ]:
        print(f"Performing {operation} {amount}")
        (
            mailbox.send(scheduler, operation, amount)
            .then_result(
                lambda balance_value: print(
                    f"Balance after {operation} is {balance_value}"
                )
            )
            .fail(
                lambda operation_error: print(
                    f"Error performing {operation}: {operation_error}"
                )
            )
        )

        mailbox.send(scheduler, "balance").then_result(
            lambda balance_value: print(f"The Balance is {balance_value}")
        )
```

### The scheduler
* A scheduler maintains a pool of threads that are used to execute messages.
* The scheduler counts the number of messages pending or in execution.
* Applications may want to wait until the scheduler count reaches zero, otherwise messages may not complete.

```Python
# FILE: python/examples/example-actor-local.py
# SPDX-License-Identifier: BSL-1.0

import time
import traeger

from example_actor_definition import make_account_actor
from example_actor_messaging import perform_operations


def main():
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    account_actor: traeger.Actor = make_account_actor(0.0)

    perform_operations(scheduler, account_actor.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
```

## Serialization
Traeger provides a library `traeger::format` with encoders and decoders for common formats like [JSON](https://en.wikipedia.org/wiki/JSON), [YAML](https://en.wikipedia.org/wiki/YAML), and [MessagePack](https://en.wikipedia.org/wiki/MessagePack).
* [JSON](https://github.com/nlohmann/json) is useful to exchange messages with a format that is human-readable.
* [MessagePack](https://github.com/msgpack/msgpack) is similar in functionality to JSON, but using a binary format that is more compact and efficient.
* [YAML](https://github.com/jbeder/yaml-cpp) is better to format configuration files.

```Python
# FILE: python/examples/example-format-serialization.py
# SPDX-License-Identifier: BSL-1.0

import traeger


def main():
    value: traeger.Value = traeger.Value(
        traeger.Map(
            {
                "name": "John",
                "age": 30,
                "married": True,
            }
        )
    )
    print(f"The original value is {value}")

    json: traeger.Format = traeger.Format("json")
    encoded = json.encode(value)
    print(f"The encoded json is {encoded}")

    decoded = json.decode(encoded)
    print(f"The decoded value is {decoded}")


if __name__ == "__main__":
    main()
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

```Python
# FILE: python/examples/example-socket-replier.py
# SPDX-License-Identifier: BSL-1.0

import time
import traeger

from example_actor_definition import make_account_actor


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://localhost:5555"

    replier: traeger.Replier = context.replier(address)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    actor: traeger.Actor = make_account_actor(0.0)
    reply_promise = replier.reply(scheduler, actor.mailbox())
    print(f"Replier listening on address: {address}")

    input("Enter a new line feed to stop")
    stopped = False

    def stop(_):
        nonlocal stopped
        stopped = True

    reply_promise.then_result(stop)
    reply_promise.set(traeger.Result.from_value(None))
    while not stopped:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
```

#### The requester
* A requester is used to send messages to a replier.
* A requester connects a socket to the network address of a replier.
* Data received by the socket is encoded then send over to the replier.
* The requester decodes responses and use them to complete messages.

```Python
# FILE: python/examples/example-socket-requester.py
# SPDX-License-Identifier: BSL-1.0

import time
import traeger

from example_actor_messaging import perform_operations


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://localhost:5555"
    format: traeger.Format = traeger.Format("json")

    requester: traeger.Requester = context.requester(address, format)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)

    print(f"Sending messages to replier on address: {address}")
    perform_operations(scheduler, requester.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
```

### The publisher/subscriber pattern
In this pattern a publisher binds to an address to multicast values grouped by topic, subscribers listen to values per topic.

#### The publisher
* A publisher is used to multicast values over a network grouped by topic.
* A publisher multicasts regardless if there are subscribers or not.

```Python
# FILE: python/examples/example-socket-publisher.py
# SPDX-License-Identifier: BSL-1.0

import time
import traeger


def broadcast(scheduler: traeger.Scheduler, publisher: traeger.Publisher, counter: int):
    print(f"Broadcasting heart-beat {counter}")
    publisher.publish(scheduler, "heart-beat", counter)
    if counter < 10:
        heart_beat(scheduler, publisher, counter + 1)


def heart_beat(
    scheduler: traeger.Scheduler,
    publisher: traeger.Publisher,
    counter: int):
    scheduler.schedule_delayed(1, lambda: broadcast(scheduler, publisher, counter))


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://*:5556"
    format: traeger.Format = traeger.Format("json")
    publisher: traeger.Publisher = context.publisher(address, format)

    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    print(f"Publishing heart-beat events on address: {address}")
    heart_beat(scheduler, publisher, 0)

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
```

#### The subscriber
* A subscriber connects to the address of a publisher.
* A subscriber listens for values ​​of topics to which it has subscribed.

```Python
# FILE: python/examples/example-socket-subscriber.py
# SPDX-License-Identifier: BSL-1.0

import time
import traeger


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://localhost:5556"
    topics: list[str] = ["heart-beat"]

    subscriber: traeger.Subscriber = context.subscriber(address, topics)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    last_heart_beat = time.time()

    def on_heart_beat(topic, value):
        nonlocal last_heart_beat
        last_heart_beat = time.time()
        print(f"{topic}: {value}")

    promise = subscriber.listen(scheduler, on_heart_beat)
    print(f"Listening for heart-beat events on address: {address}")
    listening = True

    def stop(_):
        nonlocal listening
        listening = False

    promise.then_result(stop)
    while scheduler.count() != 0 and listening:
        try:
            now = time.time()
            if now - last_heart_beat >= 2:
                print("The last heart-beat was more than 2s ago")
                promise.set(traeger.Result.from_value(None))
            time.sleep(0.010)
        except KeyboardInterrupt:
            promise.set(traeger.Result.from_value(None))


if __name__ == "__main__":
    main()
```

## Modules

Traeger provides a library `traeger::module` to define and load modules.

Actors can be instantiated in the code of shared objects, then loaded programatically.
This enables modular applications in environments where the hardware may vary.

### Loading modules

Loading a module, then retrieving a mailbox from it.

```Python
# FILE: python/examples/example-module-actor.py
# SPDX-License-Identifier: BSL-1.0

import sys
import time
import traeger

from example_actor_messaging import perform_operations


def main(argv: list[str]):
    if len(argv) < 2:
        print("Missing argument")
        return 1

    path: str = argv[1]
    print(f"Attempting to load module from path: {path}")
    configuration: traeger.Map = traeger.Map({"initial_funds": 100.0})
    module: traeger.Module = traeger.Module(path, configuration)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    perform_operations(scheduler, module.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
```

### Defining modules

The definition of a shared object, it is provided an initial configuration, and returns either a mailbox interface or an error.

```Python
# FILE: examples/example-module.cpp
// SPDX-License-Identifier: BSL-1.0

#include <iostream>

#include <traeger/value/Value.hpp>
#include <traeger/actor/Actor.hpp>

#include <traeger/module/module.h>

extern traeger::Actor make_account_actor(traeger::Float initial_funds);

extern "C" DLLEXPORT void
traeger_module_init(const traeger_map_t *configuration,
                    traeger_mailbox_interface_t **result,
                    traeger_string_t *error)
{
    if (configuration == nullptr ||
        result == nullptr ||
        error == nullptr)
    {
        return;
    }

    traeger::Float initial_funds = 0.0;
    configuration->get("initial_funds", initial_funds);
    std::cout << "initial_funds: " << initial_funds << std::endl;

    auto actor = make_account_actor(initial_funds);
    *result = actor.mailbox_interface().release();
}
```

# License
Boost Software License - Version 1.0
