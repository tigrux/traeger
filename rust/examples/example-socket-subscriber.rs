// SPDX-License-Identifier: BSL-1.0

use std::{
    cell::RefCell,
    rc::Rc,
    thread::sleep,
    time::{Duration, Instant},
};

use traeger::{Context, Scheduler, Threads};

fn main() {
    let context = Context::new();
    let address = "tcp://localhost:5556";
    let topics = ["heart-beat"];

    match context.subscriber(address, &topics) {
        Err(error) => {
            println!("Socket bind error: {error}");
        }
        Ok(subscriber) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            let last_heart_beat = Rc::new(RefCell::new(Instant::now()));

            println!("Listening for heart-beat events on address: {address}");
            let last_heart_beat_clone = last_heart_beat.clone();
            subscriber.listen(&scheduler, move |topic, variant| {
                *last_heart_beat_clone.borrow_mut() = Instant::now();
                println!("{topic}: {variant:?}");
            });

            while scheduler.count() != 0 {
                if last_heart_beat.borrow().elapsed().as_secs() >= 2 {
                    println!("The last heart-beat was more than 2s ago");
                    break;
                }
                sleep(Duration::from_millis(10));
            }
        }
    }
}
