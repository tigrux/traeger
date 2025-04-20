// SPDX-License-Identifier: BSL-1.0

#include <utility>

#include "traeger/actor/Result.hpp"
#include "traeger/actor/Scheduler.hpp"
#include "traeger/actor/Promise.hpp"
#include "traeger/socket/Publisher.hpp"

namespace traeger
{
    Publisher::Publisher(Socket &&socket,
                         const Format &format) noexcept
        : publisher_(std::move(socket)),
          format_(format)
    {
    }

    auto Publisher::publish(const Scheduler &scheduler,
                            const String &topic,
                            const Value &value) const noexcept -> Promise
    {
        auto [encoded, error] = format_.encode(value);
        if (!encoded)
        {
            Promise promise{scheduler};
            promise.set_result(Error{error});
            return promise;
        }
        return publisher_.send(scheduler, {topic, format_.name(), encoded.value()});
    }
}
