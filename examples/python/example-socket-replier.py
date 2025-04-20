# SPDX-License-Identifier: BSL-1.0

import time
import traeger

actor_definition = __import__("example-actor-definition")


def main():
    context: traeger.Context = traeger.Context()
    address: str = "tcp://localhost:5555"

    replier: traeger.Replier = context.replier(address)
    scheduler: traeger.Scheduler = traeger.Scheduler(threads_count=8)
    actor: traeger.Actor = actor_definition.make_account_actor(0.0)
    reply_promise = replier.reply(scheduler, actor.mailbox())
    print(f"Replier listening on address: {address}")

    input("Enter a new line feed to stop")
    stopped = False

    def stop(_):
        nonlocal stopped
        stopped = True

    reply_promise.then_result(stop)
    reply_promise.set(traeger.Result.from_value(None))
    while not stopped:
        time.sleep(0.010)


if __name__ == "__main__":
    main()
