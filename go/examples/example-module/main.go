// SPDX-License-Identifier: BSL-1.0

package main

import (
	"actor_definition"
	"fmt"

	traeger "github.com/tigrux/traeger/go"
)

func init() {
	traeger.ModuleInit(
		func(configuration *traeger.Map) (*traeger.BaseActor, error) {
			initial_funds := 0.0
			configuration.Get("initial_funds", &initial_funds)

			fmt.Printf("initial_funds: %f\n", initial_funds)
			actor := actor_definition.MakeAccountActor(initial_funds)
			return &actor.BaseActor, nil
		})
}

func main() {
}
