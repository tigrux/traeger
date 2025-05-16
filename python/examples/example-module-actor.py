# SPDX-License-Identifier: BSL-1.0

import sys
import time
import traeger

from example_actor_messaging import perform_operations


def main(argv: list[str]):
    if len(argv) < 2:
        print("Missing argument")
        return 1

    path: str = argv[1]
    print(f"Attempting to load module from path: {path}")
    configuration: traeger.Map = traeger.Map({"initial_funds": 100.0})
    module: traeger.Module = traeger.Module(path, configuration)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    perform_operations(scheduler, module.mailbox())

    while scheduler.count() != 0:
        time.sleep(0.010)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
