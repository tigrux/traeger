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
    * C: for interfacing with other languages like Go.
    * Go: for networking and micro-services.
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

```Go
// FILE: go/examples/example-value-semantics/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"fmt"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	list1 := traeger.MakeList([]any{2, 3, 5})
	fmt.Printf("This is list1: %v\n", list1)

	list2 := list1.Copy()
	list2.SetInt(0, 1)
	fmt.Printf("The list2 has odd numbers: %v\n", list2)

	list3 := list1.Copy()
	list3.AppendInt(7)
	fmt.Printf("The list3 has prime numbers: %v\n", list3)

	map1 := traeger.MakeMap(map[string]any{
		"odds":   list2,
		"primes": list3,
	})
	fmt.Println("A map with the numbers:")
	for key, value := range map1.Items() {
		fmt.Printf("%s: %v\n", key, value)
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

```Go
// FILE: go/examples/example-actor-definition/pkg.go
// SPDX-License-Identifier: BSL-1.0

package actor_definition

import (
	"errors"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

type Account struct {
	funds float64
}

func (account *Account) deposit(arguments *traeger.List) (any, error) {
	// simulate a costly operation
	time.Sleep(200 * time.Millisecond)

	var amount float64
	if ok, err := arguments.Unpack(&amount); !ok {
		return nil, err
	}

	if amount <= 0.0 {
		return nil, errors.New("invalid amount")
	}
	account.funds += amount
	return account.funds, nil
}

func (account *Account) debit(arguments *traeger.List) (any, error) {
	// simulate a costly operation
	time.Sleep(200 * time.Millisecond)

	var amount float64
	if ok, err := arguments.Unpack(&amount); !ok {
		return nil, err
	}

	if amount <= 0.0 {
		return nil, errors.New("invalid amount")
	}
	if account.funds < amount {
		return nil, errors.New("not enough funds")
	}
	account.funds -= amount
	return account.funds, nil
}

func (account *Account) balance(arguments *traeger.List) (any, error) {
	// simulate a costly operation
	time.Sleep(200 * time.Millisecond)

	return account.funds, nil
}

// this function acts as the factory for the Account actor
func MakeAccountActor(initialFunds float64) *traeger.Actor[Account] {
	accountActor := traeger.MakeActor(&Account{initialFunds})
	accountActor.DefineWriter("deposit", (*Account).deposit)
	accountActor.DefineWriter("debit", (*Account).debit)
	accountActor.DefineReader("balance", (*Account).balance)
	return accountActor
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

```Go
// FILE: go/examples/example-actor-messaging/pkg.go
// SPDX-License-Identifier: BSL-1.0

package actor_messaging

import (
	"fmt"

	traeger "github.com/tigrux/traeger/go"
)

type OperationAmount struct {
	operation string
	amount    float64
}

func PerformOperations(scheduler *traeger.Scheduler, mailbox *traeger.Mailbox) {
	for _, operation_amount := range []OperationAmount{
		{"deposit", 1000},
		{"deposit", 500},
		{"deposit", 0},
		{"debit", -2000},
		{"debit", 750},
		{"deposit", 250},
		{"debit", 500},
	} {
		operation := operation_amount.operation
		amount := operation_amount.amount
		fmt.Printf("Performing %s %f\n", operation, amount)
		mailbox.
			Send(scheduler, operation, amount).
			ThenResult(func(balance_value *traeger.Value) (any, error) {
				fmt.Printf("Balance after %s is %v\n", operation, balance_value)
				return nil, nil
			}).
			Fail(func(operation_error error) {
				fmt.Printf("Error performing %s: %s\n", operation, operation_error.Error())
			})

		mailbox.
			Send(scheduler, "balance").
			ThenResult(func(balance_value *traeger.Value) (any, error) {
				fmt.Printf("The Balance is %v\n", balance_value)
				return nil, nil
			})
	}

}
```

### The scheduler
* A scheduler maintains a pool of threads that are used to execute messages.
* The scheduler counts the number of messages pending or in execution.
* Applications may want to wait until the scheduler count reaches zero, otherwise messages may not complete.

```Go
// FILE: go/examples/example-actor-local/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"actor_definition"
	"actor_messaging"
	"runtime"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	scheduler := traeger.NewScheduler(8)
	actor := actor_definition.MakeAccountActor(0.0)

	actor_messaging.PerformOperations(scheduler, actor.Mailbox())

	for scheduler.Count() != 0 {
		time.Sleep(10 * time.Millisecond)
		runtime.GC()
	}
}
```

## Serialization
Traeger provides a library `traeger::format` with encoders and decoders for common formats like [JSON](https://en.wikipedia.org/wiki/JSON), [YAML](https://en.wikipedia.org/wiki/YAML), and [MessagePack](https://en.wikipedia.org/wiki/MessagePack).
* [JSON](https://github.com/nlohmann/json) is useful to exchange messages with a format that is human-readable.
* [MessagePack](https://github.com/msgpack/msgpack) is similar in functionality to JSON, but using a binary format that is more compact and efficient.
* [YAML](https://github.com/jbeder/yaml-cpp) is better to format configuration files.

```Go
// FILE: go/examples/example-format-serialization/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"fmt"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	value := traeger.MakeValue(
		traeger.MakeMap(map[string]any{
			"name":    "John",
			"age":     30,
			"married": true}))
	fmt.Printf("The original value is %v\n", value)

	json := traeger.NewFormatByName("json")
	encoded, err := json.EncodeValue(value)
	if err != nil {
		fmt.Printf("Encode error %v\n", err.Error())
		return
	}

	fmt.Printf("The encoded json is %v\n", encoded)

	decoded, err := json.DecodeString(encoded)
	if err != nil {
		fmt.Printf("Decode error %v\n", err.Error())
	}
	fmt.Printf("The decoded value is %v\n", decoded)
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

```Go
// FILE: go/examples/example-socket-replier/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"actor_definition"
	"bufio"
	"fmt"
	"os"
	"runtime"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	context := traeger.NewContext()
	address := "tcp://localhost:5555"

	replier, err := context.Replier(address)
	if err != nil {
		fmt.Printf("Socket error = %v\n", err.Error())
		return
	}

	scheduler := traeger.NewScheduler(8)
	actor := actor_definition.MakeAccountActor(0.0)
	reply_promise := replier.Reply(scheduler, actor.Mailbox())
	fmt.Printf("Replier listening on address: %v\n", address)

	fmt.Println("Enter a new line feed to stop")
	bufio.NewReader(os.Stdin).ReadString('\n')

	stopped := false
	reply_promise.ThenResult(func(value *traeger.Value) (any, error) {
		stopped = true
		return nil, nil
	})
	reply_promise.SetResult(traeger.MakeResult(nil))
	for !stopped {
		time.Sleep(10 * time.Millisecond)
		runtime.GC()
	}
}
```

#### The requester
* A requester is used to send messages to a replier.
* A requester connects a socket to the network address of a replier.
* Data received by the socket is encoded then send over to the replier.
* The requester decodes responses and use them to complete messages.

```Go
// FILE: go/examples/example-socket-requester/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"actor_messaging"
	"fmt"
	"runtime"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	context := traeger.NewContext()
	address := "tcp://localhost:5555"
	format := traeger.NewFormatByName("json")

	requester, err := context.Requester(address, format)
	if err != nil {
		fmt.Printf("Socket error = %v\n", err.Error())
		return
	}

	scheduler := traeger.NewScheduler(8)
	fmt.Printf("Sending messages to replier on address: %v\n", address)
	actor_messaging.PerformOperations(scheduler, requester.Mailbox())

	for scheduler.Count() != 0 {
		time.Sleep(10 * time.Millisecond)
		runtime.GC()
	}
}
```

### The publisher/subscriber pattern
In this pattern a publisher binds to an address to multicast values grouped by topic, subscribers listen to values per topic.

#### The publisher
* A publisher is used to multicast values over a network grouped by topic.
* A publisher multicasts regardless if there are subscribers or not.

```Go
// FILE: go/examples/example-socket-publisher/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"fmt"
	"runtime"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

func heart_beat(
	scheduler *traeger.Scheduler,
	publisher *traeger.Publisher,
	counter int64) {
	scheduler.ScheduleDelayed(
		1*time.Second,
		func() {
			fmt.Printf("Broadcasting heart-beat: %v\n", counter)
			publisher.Publish(scheduler, "heart-beat", traeger.MakeValue(counter))
			if counter < 10 {
				heart_beat(scheduler, publisher, counter+1)
			}
		},
	)
}

func main() {
	context := traeger.NewContext()
	address := "tcp://localhost:5556"
	format := traeger.NewFormatByName("json")

	publisher, err := context.Publisher(address, format)
	if err != nil {
		fmt.Printf("Socket bind error = %s\n", err)
		return
	}

	scheduler := traeger.NewScheduler(8)
	fmt.Printf("Publishing heart-beat events on address: %v\n", address)
	heart_beat(scheduler, publisher, 0)

	for scheduler.Count() != 0 {
		time.Sleep(10 * time.Millisecond)
		runtime.GC()
	}
}
```

#### The subscriber
* A subscriber connects to the address of a publisher.
* A subscriber listens for values ​​of topics to which it has subscribed.

```Go
// FILE: go/examples/example-socket-subscriber/main.go
// SPDX-License-Identifier: BSL-1.0

package main

import (
	"fmt"
	"runtime"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	context := traeger.NewContext()
	address := "tcp://localhost:5556"
	topics := []string{"heart-beat"}

	subscriber, err := context.Subscriber(address, topics)
	if err != nil {
		fmt.Printf("Socket error = %v\n", err.Error())
		return
	}

	scheduler := traeger.NewScheduler(8)
	last_heart_beat := time.Now()

	fmt.Printf("Listening for heart-beat events on address: %v\n", address)
	subscriber.Listen(scheduler,
		func(topic string, value *traeger.Value) {
			last_heart_beat = time.Now()
			fmt.Printf("%s: %v\n", topic, value)
		})

	for scheduler.Count() != 0 {
		now := time.Now()
		if now.Sub(last_heart_beat) >= 2*time.Second {
			fmt.Println("The last heart-beat was more than 2s ago")
			break
		}
		time.Sleep(10 * time.Millisecond)
		runtime.GC()
	}
}
```

# License
Boost Software License - Version 1.0
