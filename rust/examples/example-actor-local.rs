// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Scheduler, Threads};

pub mod example_actor_definition;
use example_actor_definition::make_account_actor;

pub mod example_actor_messaging;
use example_actor_messaging::perform_operations;

fn main() {
    let scheduler = Scheduler::new(Threads { count: 8 });
    let account_actor = make_account_actor(0.0);

    perform_operations(&scheduler, &account_actor.mailbox());

    while scheduler.count() != 0 {
        sleep(Duration::from_millis(10));
    }
}
