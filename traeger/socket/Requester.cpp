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
    struct Requester::impl_type
        : public std::enable_shared_from_this<impl_type>,
          public Mailbox::Interface
    {
        virtual ~impl_type() noexcept = default;

        impl_type(Socket &&socket,
                  const Format &format) noexcept
            : dealer_(std::move(socket)),
              format_(format)
        {
        }

        virtual auto
        send(const Scheduler &scheduler,
             const String &name,
             const List &arguments) noexcept -> Promise
        {
            auto [encoded, encoded_error] = format_.encode(arguments);
            if (!encoded)
            {
                Promise promise{scheduler};
                promise.set_result(Error{encoded_error});
                return promise;
            }
            return dealer_
                .send(scheduler, {name, format_.name(), encoded.value()})
                .then(
                    [impl = shared_from_this(), scheduler](const Value &value) -> Promise
                    {
                        return impl->dealer_.recv(scheduler);
                    })
                .then(
                    [impl = shared_from_this()](const Value &value) -> Result
                    {
                        const auto list = value.get_list().value();
                        traeger::String response;
                        traeger::String response_error;
                        auto [unpack_ok, unpack_error] = list.get(response, response_error);
                        if (!unpack_ok)
                        {
                            return {Error{unpack_error}};
                        }
                        if (response.empty())
                        {
                            return {Error{response_error}};
                        }
                        auto [decoded, decode_error] = impl->format_.decode(response);
                        if (!decoded)
                        {
                            return {Error{decode_error}};
                        }
                        return {decoded.value()};
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
