# SPDX-License-Identifier: BSL-1.0

import time
import traeger

from example_actor_definition import make_account_actor
from example_actor_messaging import perform_operations


def main():
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    account_actor: traeger.Actor = make_account_actor(0.0)

    perform_operations(scheduler, account_actor.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
