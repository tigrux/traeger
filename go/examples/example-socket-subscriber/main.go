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
