// SPDX-License-Identifier: BSL-1.0

package main

import (
	"actor_messaging"
	"fmt"
	"os"
	"runtime"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Missing arguments")
		return
	}

	path := os.Args[1]
	configuration := traeger.MakeMap(map[string]any{
		"initial_funds": 1000.0,
	})

	fmt.Printf("Attempting to load module from path: %s", path)
	module, err := traeger.NewModuleFromPathOrError(path, configuration)
	if err != nil {
		fmt.Printf("Module error: %s", err.Error())
		return
	}

	scheduler := traeger.NewScheduler(8)
	actor_messaging.PerformOperations(scheduler, module.Mailbox())

	for scheduler.Count() != 0 {
		time.Sleep(10 * time.Millisecond)
		runtime.GC()
	}
}
