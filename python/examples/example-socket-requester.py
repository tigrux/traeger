# SPDX-License-Identifier: BSL-1.0

import time
import traeger

from example_actor_messaging import perform_operations


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://localhost:5555"
    format: traeger.Format = traeger.Format("json")

    requester: traeger.Requester = context.requester(address, format)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)

    print(f"Sending messages to replier on address: {address}")
    perform_operations(scheduler, requester.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
