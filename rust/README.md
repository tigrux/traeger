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

```Rust
// FILE: rust/examples/example-value-semantics.rs
// SPDX-License-Identifier: BSL-1.0

use traeger::{Append, SetIndex, list, map};

fn main() {
    let list1 = list!(2, 3, 5);
    println!("This is list1: {list1}");

    let mut list2 = list1.clone();
    list2.set(0, 1);
    list2.append(7);
    println!("The listt2 has odd numbers: {list2}");

    let mut list3 = list1.clone();
    list3.append(7);
    println!("The list3 has prime numbers: {list3}");

    let map = map!(
        "odds" => &list2,
        "primes" => &list3
    );
    println!("A map with the numbers:");
    for (key, value) in &map {
        println!("{key}: {value}");
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

```Rust
// FILE: rust/examples/example_actor_definition/mod.rs
// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Actor, Float, List, Variant, unpack};

pub struct Account {
    funds: Float,
}

impl Account {
    pub fn new(initial_funds: f64) -> Self {
        Self {
            funds: initial_funds,
        }
    }

    // this method is mut i.e. it modifies the instance
    fn deposit(&mut self, arguments: List) -> Result<Variant, String> {
        // simulate a costly operation
        sleep(Duration::from_millis(200));
        let (amount,) = unpack!(arguments, 0 => Float)?;
        if amount <= 0.0 {
            Err(format!("invalid amount"))
        } else {
            self.funds += amount;
            Ok(Variant::Float(self.funds))
        }
    }

    // this method is mut i.e. it modifies the instance
    fn debit(&mut self, arguments: List) -> Result<Variant, String> {
        // simulate a costly operation
        sleep(Duration::from_millis(200));
        let (amount,) = unpack!(arguments, 0 => Float)?;
        if amount <= 0.0 {
            Err(format!("invalid amount"))
        } else if self.funds < amount {
            Err(format!("not enough funds"))
        } else {
            self.funds -= amount;
            Ok(Variant::Float(self.funds))
        }
    }

    // this method is non-mut i.e. it does not modify the instance
    fn balance(&self, _arguments: List) -> Result<Variant, String> {
        // simulate a costly operation
        sleep(Duration::from_millis(200));
        Ok(Variant::Float(self.funds))
    }
}

// this function acts as the factory for the Account actor
pub fn make_account_actor(initial_funds: Float) -> Actor<Account> {
    Actor::new(Account::new(initial_funds))
        .define_writer("deposit", Account::deposit)
        .define_writer("debit", Account::debit)
        .define_reader("balance", Account::balance)
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

```Rust
// FILE: rust/examples/example_actor_messaging/mod.rs
// SPDX-License-Identifier: BSL-1.0

use traeger::{Mailbox, Scheduler, Variant, list};

pub fn perform_operations(scheduler: &Scheduler, mailbox: &Mailbox) {
    for (operation, amount) in [
        ("deposit", 1000.0),
        ("deposit", 500.0),
        ("deposit", 0.0),
        ("debit", -2000.0),
        ("debit", 750.0),
        ("deposit", 250.0),
        ("debit", 500.0),
    ] {
        println!("Performing {operation} {amount}");
        mailbox
            .send(scheduler, operation, &list!(amount))
            .then_result(move |balance_value| {
                println!("Balance after {operation} is {balance_value}");
                return Ok(Variant::Null);
            })
            .fail(move |operation_error| {
                println!("Error performing {operation}: {operation_error}")
            })
    }

    mailbox
        .send(&scheduler, "balance", &list!())
        .then_result(|balance_value| {
            println!("The balance is {balance_value}");
            return Ok(Variant::Null);
        });
}
```

### The scheduler
* A scheduler maintains a pool of threads that are used to execute messages.
* The scheduler counts the number of messages pending or in execution.
* Applications may want to wait until the scheduler count reaches zero, otherwise messages may not complete.

```Rust
// FILE: rust/examples/example-actor-local.rs
// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Scheduler, Threads};

pub mod example_actor_definition;
use example_actor_definition::make_account_actor;

pub mod example_actor_messaging;
use example_actor_messaging::perform_operations;

fn main() {
    let scheduler = Scheduler::new(Threads { count: 8 });
    let account_actor = make_account_actor(0.0);

    perform_operations(&scheduler, &account_actor.mailbox());

    while scheduler.count() != 0 {
        sleep(Duration::from_millis(10));
    }
}
```

## Serialization
Traeger provides a library `traeger::format` with encoders and decoders for common formats like [JSON](https://en.wikipedia.org/wiki/JSON), [YAML](https://en.wikipedia.org/wiki/YAML), and [MessagePack](https://en.wikipedia.org/wiki/MessagePack).
* [JSON](https://github.com/nlohmann/json) is useful to exchange messages with a format that is human-readable.
* [MessagePack](https://github.com/msgpack/msgpack) is similar in functionality to JSON, but using a binary format that is more compact and efficient.
* [YAML](https://github.com/jbeder/yaml-cpp) is better to format configuration files.

```Rust
// FILE: rust/examples/example-format-serialization.rs
// SPDX-License-Identifier: BSL-1.0

use std::str::from_utf8_unchecked;

use traeger::{Format, Variant, map};

fn main() {
    let variant = Variant::Map(map!(
        "name" => "John",
        "age" => 30,
        "married" => true
    ));
    println!("The original variant is {variant:?}");

    let json = Format::by_name("json").unwrap();
    match json.encode(variant) {
        Err(error) => {
            println!("Encode error {error}");
        }
        Ok(data) => {
            println!("The encoded json is {}", unsafe {
                from_utf8_unchecked(&data)
            });
            match json.decode(&data) {
                Err(error) => {
                    println!("Decode error {error}");
                }
                Ok(variant) => {
                    println!("The decoded variant is {variant:?}");
                }
            }
        }
    }
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

```Rust
// FILE: rust/examples/example-socket-replier.rs
// SPDX-License-Identifier: BSL-1.0

use std::io::{BufRead, stdin};

use traeger::{Context, Scheduler, Threads, Variant};

pub mod example_actor_definition;
use example_actor_definition::make_account_actor;

fn main() {
    let context = Context::new();
    let address = "tcp://*:5555";

    match context.replier(address) {
        Err(error) => {
            println!("Socket error = {error}");
        }
        Ok(replier) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let actor = make_account_actor(0.0);
            let reply_promise = replier.reply(&scheduler, actor.mailbox());

            println!("Enter a new line feed to stop");
            let _dummy_result = stdin().lock().lines().next();
            reply_promise.set(Ok(Variant::Null));
        }
    }
}
```

#### The requester
* A requester is used to send messages to a replier.
* A requester connects a socket to the network address of a replier.
* Data received by the socket is encoded then send over to the replier.
* The requester decodes responses and use them to complete messages.

```Rust
// FILE: rust/examples/example-socket-requester.rs
// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Context, Format, Scheduler, Threads};

pub mod example_actor_messaging;
use example_actor_messaging::perform_operations;

fn main() {
    let context = Context::new();
    let address = "tcp://localhost:5555";
    let format = Format::by_name("json").unwrap();

    match context.requester(address, &format) {
        Err(error) => {
            println!("Socket error {error}");
        }
        Ok(requester) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let mailbox = requester.mailbox();

            println!("Sending messages to replier on address: {address}");
            perform_operations(&scheduler, &mailbox);

            while scheduler.count() != 0 {
                sleep(Duration::from_millis(10));
            }
        }
    }
}
```

### The publisher/subscriber pattern
In this pattern a publisher binds to an address to multicast values grouped by topic, subscribers listen to values per topic.

#### The publisher
* A publisher is used to multicast values over a network grouped by topic.
* A publisher multicasts regardless if there are subscribers or not.

```Rust
// FILE: rust/examples/example-socket-publisher.rs
// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Context, Format, Int, Publisher, Scheduler, Threads, Variant};

fn heart_beat(scheduler: &Scheduler, publisher: &Publisher, counter: Int) {
    let scheduler_clone = scheduler.clone();
    let publisher_clone = publisher.clone();
    scheduler.schedule_delayed(Duration::from_secs(1), move || {
        println!("Broadcasting heart-beat: {counter}");
        publisher_clone.publish(&scheduler_clone, "heart-beat", Variant::Int(counter));
        if counter < 10 {
            heart_beat(&scheduler_clone, &publisher_clone, counter + 1);
        }
    });
}

fn main() {
    let context = Context::new();
    let address = "tcp://*:5556";
    let format = Format::by_name("json").unwrap();

    match context.publisher(address, &format) {
        Err(error) => {
            println!("Socket bind error: {error}");
        }
        Ok(publisher) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            println!("Publishing heart-beat events on address: {address}");
            heart_beat(&scheduler, &publisher, 0);

            while scheduler.count() != 0 {
                sleep(std::time::Duration::from_millis(10));
            }
        }
    }
}
```

#### The subscriber
* A subscriber connects to the address of a publisher.
* A subscriber listens for values ​​of topics to which it has subscribed.

```Rust
// FILE: rust/examples/example-socket-subscriber.rs
// SPDX-License-Identifier: BSL-1.0

use std::{
    cell::RefCell,
    rc::Rc,
    thread::sleep,
    time::{Duration, Instant},
};

use traeger::{Context, Scheduler, Threads};

fn main() {
    let context = Context::new();
    let address = "tcp://localhost:5556";
    let topics = ["heart-beat"];

    match context.subscriber(address, &topics) {
        Err(error) => {
            println!("Socket bind error: {error}");
        }
        Ok(subscriber) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let last_heart_beat = Rc::new(RefCell::new(Instant::now()));

            println!("Listening for heart-beat events on address: {address}");
            let last_heart_beat_clone = last_heart_beat.clone();
            subscriber.listen(&scheduler, move |topic, variant| {
                *last_heart_beat_clone.borrow_mut() = Instant::now();
                println!("{topic}: {variant:?}");
            });

            while scheduler.count() != 0 {
                if last_heart_beat.borrow().elapsed().as_secs() >= 2 {
                    println!("The last heart-beat was more than 2s ago");
                    break;
                }
                sleep(Duration::from_millis(10));
            }
        }
    }
}
```

## Modules

Traeger provides a library `traeger::module` to define and load modules.

Actors can be instantiated in the code of shared objects, then loaded programatically.
This enables modular applications in environments where the hardware may vary.

### Loading modules

Loading a module, then retrieving a mailbox from it.

```Rust
// FILE: rust/examples/example-module-actor.rs
// SPDX-License-Identifier: BSL-1.0

use std::{env, thread::sleep, time::Duration};

use traeger::{Module, Scheduler, Threads, map};

pub mod example_actor_messaging;
use example_actor_messaging::perform_operations;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Missing argument");
        return;
    }

    let path = &args[1];
    println!("Attempting to load module from path: {path}");

    match Module::from_path(&path, &map!("initial_funds" => 1000.0)) {
        Err(error) => {
            println!("Module error: {error}");
        }
        Ok(module) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let mailbox = module.mailbox();
            perform_operations(&scheduler, &mailbox);

            while scheduler.count() != 0 {
                sleep(Duration::from_millis(10));
            }
        }
    }
}
```

### Defining modules

The definition of a shared object, it is provided an initial configuration, and returns either a mailbox interface or an error.

```Rust
// FILE: rust/examples/example-module.rs
// SPDX-License-Identifier: BSL-1.0

use traeger::{unpack, Float};

pub mod example_actor_definition;
use example_actor_definition::make_account_actor;

traeger::module_init!(|map| {
    let initial_funds = match unpack!(map, "initial_funds" => Float) {
        Ok((value,)) => value,
        Err(_) => 0.0,
    };

    println!("initial_funds: {initial_funds}");
    let actor = make_account_actor(initial_funds);
    Ok(actor)
});

#[allow(dead_code)]
fn main() {}
```

# License
Boost Software License - Version 1.0
