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
