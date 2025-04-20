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
