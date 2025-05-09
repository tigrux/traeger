# SPDX-License-Identifier: BSL-1.0

import time
import traeger

actor_definition = __import__("example-actor-definition")
actor_messaging = __import__("example-actor-messaging")


def main():
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    account_actor: traeger.Actor = actor_definition.make_account_actor(0.0)

    actor_messaging.perform_operations(scheduler, account_actor.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
