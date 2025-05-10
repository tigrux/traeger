// SPDX-License-Identifier: BSL-1.0

use traeger::{Mailbox, Scheduler, Variant, list};

pub fn perform_operations(scheduler: &Scheduler, mailbox: &Mailbox) {
    for (operation, amount) in [
        ("deposit", 1000.0),
        ("deposit", 500.0),
        ("deposit", 0.0),
        ("debit", -2000.0),
        ("debit", 750.0),
        ("deposit", 250.0),
        ("debit", 500.0),
    ] {
        println!("Performing {operation} {amount}");
        mailbox
            .send(scheduler, operation, &list!(amount))
            .then_result(move |balance_value| {
                println!("Balance after {operation} is {balance_value}");
                return Ok(Variant::Null);
            })
            .fail(move |operation_error| {
                println!("Error performing {operation}: {operation_error}")
            })
    }

    mailbox
        .send(&scheduler, "balance", &list!())
        .then_result(|balance_value| {
            println!("The balance is {balance_value}");
            return Ok(Variant::Null);
        });
}
