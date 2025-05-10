// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Context, Format, Scheduler, Threads};

pub mod example_actor_messaging;
use example_actor_messaging::perform_operations;

fn main() {
    let context = Context::new();
    let address = "tcp://localhost:5555";
    let format = Format::by_name("json").unwrap();

    match context.requester(address, &format) {
        Err(error) => {
            println!("Socket error {error}");
        }
        Ok(requester) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let mailbox = requester.mailbox();

            println!("Sending messages to replier on address: {address}");
            perform_operations(&scheduler, &mailbox);

            while scheduler.count() != 0 {
                sleep(Duration::from_millis(10));
            }
        }
    }
}
