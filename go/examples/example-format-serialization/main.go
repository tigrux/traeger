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
