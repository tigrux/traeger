// SPDX-License-Identifier: BSL-1.0

use std::{env, thread::sleep, time::Duration};

use traeger::{Module, Scheduler, Threads, map};

pub mod example_actor_messaging;
use example_actor_messaging::perform_operations;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Missing argument");
        return;
    }

    let path = &args[1];
    println!("Attempting to load module from path: {path}");

    match Module::from_path(&path, &map!("initial_funds" => 1000.0)) {
        Err(error) => {
            println!("Module error: {error}");
        }
        Ok(module) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let mailbox = module.mailbox();
            perform_operations(&scheduler, &mailbox);

            while scheduler.count() != 0 {
                sleep(Duration::from_millis(10));
            }
        }
    }
}
