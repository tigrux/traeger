# SPDX-License-Identifier: BSL-1.0

import time
import traeger


def broadcast(scheduler: traeger.Scheduler, publisher: traeger.Publisher, counter: int):
    print(f"Broadcasting heart-beat {counter}")
    publisher.publish(scheduler, "heart-beat", counter)
    if counter < 10:
        heart_beat(scheduler, publisher, counter + 1)


def heart_beat(
    scheduler: traeger.Scheduler, publisher: traeger.Publisher, counter: int
):
    scheduler.schedule_delayed(1, lambda: broadcast(scheduler, publisher, counter))


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://*:5556"
    format: traeger.Format = traeger.Format("json")
    publisher: traeger.Publisher = context.publisher(address, format)

    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    print(f"Publishing heart-beat events on address: {address}")
    heart_beat(scheduler, publisher, 0)

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
