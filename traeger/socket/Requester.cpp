// SPDX-License-Identifier: BSL-1.0

#include <utility>
#include <memory>

#include "traeger/format/Format.hpp"
#include "traeger/actor/Result.hpp"
#include "traeger/actor/Scheduler.hpp"
#include "traeger/actor/Promise.hpp"
#include "traeger/actor/Mailbox.hpp"
#include "traeger/socket/Socket.hpp"
#include "traeger/socket/Requester.hpp"

namespace traeger
{
    struct Requester::impl_type final
        : std::enable_shared_from_this<impl_type>,
          Mailbox::Interface
    {
        ~impl_type() noexcept override = default;

        impl_type(Socket &&socket,
                  Format format) noexcept
            : dealer_(std::move(socket)),
              format_(std::move(format))
        {
        }

        Promise
        send(const Scheduler &scheduler,
             const String &name,
             const List &arguments) noexcept override
        {
            auto [encoded, encoded_error] = format_.encode(arguments);
            if (!encoded)
            {
                Promise promise{scheduler};
                promise.set_result(Result{Error{encoded_error}});
                return promise;
            }
            return dealer_
                .send(scheduler, {name, format_.name(), encoded.value()})
                .then(
                    [impl = shared_from_this(), scheduler](const Value &) -> Promise
                    {
                        return impl->dealer_.recv(scheduler);
                    })
                .then(
                    [impl = shared_from_this()](const Value &value) -> Result
                    {
                        const auto list = value.get_list().value();
                        String response;
                        String response_error;
                        if (auto [unpack_ok, unpack_error] = list.unpack(response, response_error);
                            !unpack_ok)
                        {
                            return Result{Error{unpack_error}};
                        }
                        if (response.empty())
                        {
                            return Result{Error{response_error}};
                        }
                        auto [decoded, decode_error] = impl->format_.decode(response);
                        if (!decoded)
                        {
                            return Result{Error{decode_error}};
                        }
                        return Result{decoded.value()};
                    });
        }

    private:
        Socket dealer_;
        Format format_;
    };

    Requester::Requester(Socket &&socket,
                         const Format &format) noexcept
        : impl_(std::make_shared<impl_type>(std::move(socket), format))
    {
    }

    auto Requester::mailbox() const noexcept -> Mailbox
    {
        return Mailbox{impl_};
    }
}
