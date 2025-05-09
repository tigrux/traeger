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
