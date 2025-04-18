#include <immer/map.hpp>
#include <immer/map_transient.hpp>

#include "traeger/actor/Group.hpp"

namespace traeger
{
    struct Group::impl_type
    {
        using MemberMap = immer::map<String, Mailbox>;

        struct mailbox_type : public Mailbox::Interface
        {
            virtual ~mailbox_type() noexcept = default;

            auto send(const Scheduler &scheduler,
                      const String &path,
                      const List &arguments) noexcept -> Promise
            {
                const auto promise = Promise{scheduler};
                const auto pos = path.find('/', 0);
                if (pos == std::string_view::npos)
                {
                    promise.set_result(Error{"invalid path " + path});
                    return promise;
                }

                const auto member = path.substr(0, pos);
                const auto *mailbox_ptr = members_.find(member);
                if (mailbox_ptr == nullptr)
                {
                    promise.set_result(Error{"no such group member " + member});
                    return promise;
                }

                const auto method = path.substr(pos + 1);
                const auto send_promise = mailbox_ptr->send(scheduler, method, arguments);
                send_promise.then(
                    [promise, member, method](const Value &value) -> Result
                    {
                        if (const auto optional = value.get_map(); optional)
                        {
                            auto map = optional.value();
                            String source;
                            if (auto [ok, error] = map.get("source", source); ok)
                            {
                                map.set("source", member + "/" + source);
                                promise.set_result({Result{Value{map}}});
                                return {};
                            }
                        }
                        promise.set_result(Value{
                            make_map(
                                "source", member + "/" + method,
                                "value", value,
                                "error", nullptr)});
                        return {};
                    });
                send_promise.fail(
                    [promise, member, method](const Error &error)
                    {
                        promise.set_result(Value{
                            make_map(
                                "source", member + "/" + method,
                                "value", nullptr,
                                "error", Error{error})});
                    });
                return promise;
            }

            mailbox_type(MemberMap::transient_type members) noexcept
                : members_(members.persistent())
            {
            }

        private:
            MemberMap members_;
        };

        auto add(const String &name, const Mailbox &mailbox) noexcept -> void
        {
            members_.set(name, mailbox);
        }

        auto mailbox() const noexcept -> std::shared_ptr<mailbox_type>
        {
            return std::make_shared<mailbox_type>(members_);
        }

        auto find(const String &member) const noexcept -> std::optional<Mailbox>
        {
            if (const auto *iter = members_.find(member); iter != nullptr)
            {
                return *iter;
            }
            return std::nullopt;
        }

    private:
        MemberMap::transient_type members_;
    };

    Group::Group() noexcept : impl_(std::make_shared<impl_type>())
    {
    }

    auto Group::add(const String &name, const Mailbox &mailbox) const noexcept -> void
    {
        impl_->add(name, mailbox);
    }

    auto Group::mailbox() const noexcept -> Mailbox
    {
        return Mailbox{impl_->mailbox()};
    }

    auto Group::find(const String &member) const noexcept -> std::optional<Mailbox>
    {
        return impl_->find(member);
    }
} // namespace traeger