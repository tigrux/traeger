// SPDX-License-Identifier: BSL-1.0

use std::{thread::sleep, time::Duration};

use traeger::{Context, Format, Int, Publisher, Scheduler, Threads, Variant};

fn heart_beat(scheduler: &Scheduler, publisher: &Publisher, counter: Int) {
    let scheduler_clone = scheduler.clone();
    let publisher_clone = publisher.clone();
    scheduler.schedule_delayed(Duration::from_secs(1), move || {
        println!("Broadcasting heart-beat: {counter}");
        publisher_clone.publish(&scheduler_clone, "heart-beat", Variant::Int(counter));
        if counter < 10 {
            heart_beat(&scheduler_clone, &publisher_clone, counter + 1);
        }
    });
}

fn main() {
    let context = Context::new();
    let address = "tcp://*:5556";
    let format = Format::by_name("json").unwrap();

    match context.publisher(address, &format) {
        Err(error) => {
            println!("Socket bind error: {error}");
        }
        Ok(publisher) => {
            let scheduler = Scheduler::new(Threads { count: 8 });
            println!("Publishing heart-beat events on address: {address}");
            heart_beat(&scheduler, &publisher, 0);

            while scheduler.count() != 0 {
                sleep(std::time::Duration::from_millis(10));
            }
        }
    }
}
