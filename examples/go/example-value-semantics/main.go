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
