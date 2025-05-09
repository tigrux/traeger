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
