// SPDX-License-Identifier: BSL-1.0

package actor_definition

import (
	"errors"
	"time"

	traeger "github.com/tigrux/traeger/go"
)

type Account struct {
	funds float64
}

func (account *Account) deposit(arguments *traeger.List) (any, error) {
	// simulate a costly operation
	time.Sleep(200 * time.Millisecond)

	var amount float64
	if ok, err := arguments.Get(&amount); !ok {
		return nil, err
	}

	if amount <= 0.0 {
		return nil, errors.New("invalid amount")
	}
	account.funds += amount
	return account.funds, nil
}

func (account *Account) debit(arguments *traeger.List) (any, error) {
	// simulate a costly operation
	time.Sleep(200 * time.Millisecond)

	var amount float64
	if ok, err := arguments.Get(&amount); !ok {
		return nil, err
	}

	if amount <= 0.0 {
		return nil, errors.New("invalid amount")
	}
	if account.funds < amount {
		return nil, errors.New("not enough funds")
	}
	account.funds -= amount
	return account.funds, nil
}

func (account *Account) balance(arguments *traeger.List) (any, error) {
	// simulate a costly operation
	time.Sleep(200 * time.Millisecond)

	return account.funds, nil
}

func MakeAccountActor(initialFunds float64) *traeger.Actor[Account] {
	accountActor := traeger.MakeActor(&Account{initialFunds})
	accountActor.DefineWriter("deposit", (*Account).deposit)
	accountActor.DefineWriter("debit", (*Account).debit)
	accountActor.DefineReader("balance", (*Account).balance)
	return accountActor
}
