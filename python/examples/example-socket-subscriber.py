# SPDX-License-Identifier: BSL-1.0

import time
import traeger


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://localhost:5556"
    topics: list[str] = ["heart-beat"]

    subscriber: traeger.Subscriber = context.subscriber(address, topics)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    last_heart_beat = time.time()

    def on_heart_beat(topic, value):
        nonlocal last_heart_beat
        last_heart_beat = time.time()
        print(f"{topic}: {value}")

    promise = subscriber.listen(scheduler, on_heart_beat)
    print(f"Listening for heart-beat events on address: {address}")
    listening = True

    def stop(_):
        nonlocal listening
        listening = False

    promise.then_result(stop)
    while scheduler.count() != 0 and listening:
        try:
            now = time.time()
            if now - last_heart_beat >= 2:
                print("The last heart-beat was more than 2s ago")
                promise.set(traeger.Result.from_value(None))
            time.sleep(0.010)
        except KeyboardInterrupt:
            promise.set(traeger.Result.from_value(None))


if __name__ == "__main__":
    main()
