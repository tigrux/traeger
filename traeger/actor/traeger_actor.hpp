// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "traeger/actor/actor.h"
#include "traeger/actor/Result.hpp"
#include "traeger/actor/Scheduler.hpp"
#include "traeger/actor/StatelessActor.hpp"
#include "traeger/actor/Queue.hpp"
#include "traeger/actor/Mailbox.hpp"
#include "traeger/actor/Promise.hpp"

namespace traeger
{
    inline auto cast(const traeger_result_t *result) -> const Result &
    {
        return *static_cast<const Result *>(result);
    }

    inline auto cast(traeger_result_t *result) -> Result &
    {
        return *static_cast<Result *>(result);
    }

    inline auto cast(const traeger_function_t *function) noexcept -> const Function &
    {
        return *static_cast<const Function *>(function);
    }

    inline auto cast(const traeger_scheduler_t *scheduler) noexcept -> const Scheduler &
    {
        return *static_cast<const Scheduler *>(scheduler);
    }

    inline auto cast(const traeger_promise_t *promise) noexcept -> const Promise &
    {
        return *static_cast<const Promise *>(promise);
    }

    inline auto cast(traeger_promise_t *promise) noexcept -> Promise &
    {
        return *static_cast<Promise *>(promise);
    }

    inline auto cast(const traeger_mailbox_t *mailbox) noexcept -> const Mailbox &
    {
        return *static_cast<const Mailbox *>(mailbox);
    }

    inline auto cast(const traeger_actor_t *actor) noexcept -> const StatelessActor &
    {
        return *static_cast<const StatelessActor *>(actor);
    }

    inline auto cast(const traeger_queue_t *actor) noexcept -> const Queue &
    {
        return *static_cast<const Queue *>(actor);
    }
}
