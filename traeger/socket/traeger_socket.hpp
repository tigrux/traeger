// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "traeger/socket/socket.h"
#include "traeger/socket/Context.hpp"
#include "traeger/socket/Replier.hpp"
#include "traeger/socket/Requester.hpp"
#include "traeger/socket/Publisher.hpp"
#include "traeger/socket/Subscriber.hpp"

namespace traeger
{
    inline auto cast(const traeger_context_t *context) noexcept -> const Context &
    {
        return *static_cast<const Context *>(context);
    }

    inline auto cast(const traeger_replier_t *replier) noexcept -> const Replier &
    {
        return *static_cast<const Replier *>(replier);
    }

    inline auto cast(const traeger_requester_t *requester) noexcept -> const Requester &
    {
        return *static_cast<const Requester *>(requester);
    }

    inline auto cast(const traeger_publisher_t *publisher) noexcept -> const Publisher &
    {
        return *static_cast<const Publisher *>(publisher);
    }

    inline auto cast(const traeger_subscriber_t *subscriber) noexcept -> const Subscriber &
    {
        return *static_cast<const Subscriber *>(subscriber);
    }
}
