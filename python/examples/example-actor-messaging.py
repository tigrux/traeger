# SPDX-License-Identifier: BSL-1.0

import traeger


def perform_operations(scheduler: traeger.Scheduler, mailbox: traeger.Mailbox) -> None:
    for operation, amount in [
        ("deposit", 1000),
        ("deposit", 500),
        ("deposit", 0),
        ("debit", -2000),
        ("debit", 750),
        ("deposit", 250),
        ("debit", 500),
    ]:
        print(f"Performing {operation} {amount}")
        (
            mailbox.send(scheduler, operation, amount)
            .then_result(
                lambda balance_value: print(
                    f"Balance after {operation} is {balance_value}"
                )
            )
            .fail(
                lambda operation_error: print(
                    f"Error performing {operation}: {operation_error}"
                )
            )
        )

        mailbox.send(scheduler, "balance").then_result(
            lambda balance_value: print(f"The Balance is {balance_value}")
        )
