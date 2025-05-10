// SPDX-License-Identifier: BSL-1.0

use std::io::{BufRead, stdin};

use traeger::{Context, Scheduler, Threads, Variant};

pub mod example_actor_definition;
use example_actor_definition::make_account_actor;

fn main() {
    let context = Context::new();
    let address = "tcp://*:5555";

    match context.replier(address) {
        Err(error) => {
            println!("Socket error = {error}");
        }
        Ok(replier) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let actor = make_account_actor(0.0);
            let reply_promise = replier.reply(&scheduler, actor.mailbox());

            println!("Enter a new line feed to stop");
            let _dummy_result = stdin().lock().lines().next();
            reply_promise.set(Ok(Variant::Null));
        }
    }
}
