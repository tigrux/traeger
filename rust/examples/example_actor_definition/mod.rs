// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Actor, Float, List, Variant, unpack};

pub struct Account {
    funds: Float,
}

impl Account {
    pub fn new(initial_funds: f64) -> Self {
        Self {
            funds: initial_funds,
        }
    }

    // this method is mut i.e. it modifies the instance
    fn deposit(&mut self, arguments: List) -> Result<Variant, String> {
        // simulate a costly operation
        sleep(Duration::from_millis(200));
        let (amount,) = unpack!(arguments, 0 => Float)?;
        if amount <= 0.0 {
            Err(format!("invalid amount"))
        } else {
            self.funds += amount;
            Ok(Variant::Float(self.funds))
        }
    }

    // this method is mut i.e. it modifies the instance
    fn debit(&mut self, arguments: List) -> Result<Variant, String> {
        // simulate a costly operation
        sleep(Duration::from_millis(200));
        let (amount,) = unpack!(arguments, 0 => Float)?;
        if amount <= 0.0 {
            Err(format!("invalid amount"))
        } else if self.funds < amount {
            Err(format!("not enough funds"))
        } else {
            self.funds -= amount;
            Ok(Variant::Float(self.funds))
        }
    }

    // this method is non-mut i.e. it does not modify the instance
    fn balance(&self, _arguments: List) -> Result<Variant, String> {
        // simulate a costly operation
        sleep(Duration::from_millis(200));
        Ok(Variant::Float(self.funds))
    }
}

// this function acts as the factory for the Account actor
pub fn make_account_actor(initial_funds: Float) -> Actor<Account> {
    Actor::new(Account::new(initial_funds))
        .define_writer("deposit", Account::deposit)
        .define_writer("debit", Account::debit)
        .define_reader("balance", Account::balance)
}
