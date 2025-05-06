// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <traeger/actor/Mailbox.hpp>

#include <memory>

namespace traeger
{
    struct Format;

    struct Socket;

    struct Requester
    {

        auto mailbox() const noexcept -> Mailbox;

    private:
        Requester(Socket &&socket,
                  const Format &format) noexcept;
        friend struct Context;

        struct impl_type;
        std::shared_ptr<impl_type> impl_;
    };
}

struct traeger_requester_t final : traeger::Requester
{
};
