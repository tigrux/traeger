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
