# SPDX-License-Identifier: BSL-1.0

import time
import traeger


class Account(traeger.Actor):
    def __init__(self, initial_funds):
        self.funds = initial_funds

    def deposit(self, amount: float) -> float:
        # simulate a costly operation
        time.sleep(0.200)
        if amount <= 0:
            raise RuntimeError("invalid amount")
        self.funds += amount
        return self.funds

    def debit(self, amount: float) -> float:
        # simulate a costly operation
        time.sleep(0.200)
        if amount <= 0:
            raise RuntimeError("invalid amount")
        if self.funds < amount:
            raise RuntimeError("not enough funds")
        self.funds -= amount
        return self.funds

    def balance(self) -> float:
        # simulate a costly operation
        time.sleep(0.200)
        return self.funds

def make_account_actor(initial_funds: float) -> traeger.Actor:
    account_actor = traeger.make_actor(Account, initial_funds)
    account_actor.define_writer("deposit", Account.deposit)
    account_actor.define_writer("debit", Account.debit)
    account_actor.define_reader("balance", Account.balance)
    return account_actor

