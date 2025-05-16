// SPDX-License-Identifier: BSL-1.0

#include <mutex>
#include <shared_mutex>
#include <utility>
#include <memory>
#include <queue>
#include <variant>

#include <immer/map.hpp>
#include <immer/map_transient.hpp>

#include "traeger/actor/Promise.hpp"
#include "traeger/actor/Mailbox.hpp"
#include "traeger/actor/StatelessActor.hpp"

namespace traeger
{
    struct StatelessActor::impl_type
    {
        enum class concurrency_type : int
        {
            EXCLUSIVE = 0,
            SHARED = 1,
        };

        struct lock_type
        {
            using unique_lock = std::unique_lock<std::shared_mutex>;

            using shared_lock = std::shared_lock<std::shared_mutex>;

            lock_type(concurrency_type concurrency,
                      std::shared_mutex &mutex) noexcept
            {
                switch (concurrency)
                {
                case concurrency_type::EXCLUSIVE:
                    lock_.emplace<unique_lock>(mutex, std::try_to_lock);
                    break;
                case concurrency_type::SHARED:
                    lock_.emplace<shared_lock>(mutex, std::try_to_lock);
                    break;
                }
            }

            operator bool() const noexcept
            {
                return std::visit(
                    [](const auto &lock)
                    { return bool(lock); },
                    lock_);
            }

        private:
            std::variant<unique_lock, shared_lock> lock_;
        };

        struct task_type
        {
            concurrency_type concurrency;
            Work work;
        };

        struct queue_impl_type
        {
            auto push(task_type &&task) noexcept -> void
            {
                std::lock_guard tasks_lock{tasks_mutex_};
                tasks_.emplace(std::move(task));
            }

            auto schedule_next(const Scheduler &scheduler,
                               const std::shared_ptr<queue_impl_type> &impl) noexcept -> void
            {
                std::unique_lock tasks_lock{tasks_mutex_};
                if (!tasks_.empty())
                {
                    scheduler.schedule(
                        [scheduler, impl]
                        {
                            impl->try_to_execute_next(scheduler);
                            impl->schedule_next(scheduler, impl);
                        });
                }
            }

        private:
            auto try_to_execute_next(const Scheduler &scheduler) noexcept -> void
            {
                std::unique_lock tasks_lock{tasks_mutex_};
                if (!tasks_.empty())
                {
                    auto &&next = std::move(tasks_.front());
                    if (auto lock = lock_type{next.concurrency, execution_mutex_}; lock)
                    {
                        auto work = std::move(next).work;
                        tasks_.pop();
                        tasks_lock.unlock();
                        work();
                    }
                }
            }

        private:
            std::shared_mutex execution_mutex_;
            std::mutex tasks_mutex_;
            std::queue<task_type> tasks_;
        };

        using map_type = immer::map<String, std::pair<concurrency_type, Function>>;

        struct mailbox_impl_type
            : public Mailbox::Interface
        {
            virtual ~mailbox_impl_type() noexcept = default;

            mailbox_impl_type(const std::shared_ptr<queue_impl_type> &queue,
                              map_type::transient_type functions)
                : queue_(queue),
                  functions_(std::move(functions).persistent())
            {
            }

            Promise
            send(const Scheduler &scheduler,
                 const String &name,
                 const List &arguments) noexcept override
            {
                Promise promise{scheduler};
                if (const auto *iter = functions_.find(name); iter)
                {
                    const auto &[concurrency, function] = *iter;
                    queue_->push(
                        {concurrency,
                         [promise, function = function, arguments]
                         {
                             promise.set_result(function(arguments));
                         }});
                    queue_->schedule_next(scheduler, queue_);
                }
                else
                {
                    promise.set_result(Error{"no such actor method " + name});
                }
                return promise;
            }

        private:
            std::shared_ptr<queue_impl_type> queue_;
            map_type functions_;
        };

        impl_type() noexcept
            : queue_(std::make_shared<queue_impl_type>())
        {
        }

        auto define(const String &name,
                    concurrency_type concurrency,
                    const Function &function) noexcept -> void
        {
            functions_.set(name, std::make_pair(concurrency, function));
        }

        auto mailbox() const noexcept -> std::unique_ptr<mailbox_impl_type>
        {
            return std::make_unique<mailbox_impl_type>(queue_, functions_);
        }

    private:
        std::shared_ptr<queue_impl_type> queue_;
        map_type::transient_type functions_;
    };

    StatelessActor::~StatelessActor() noexcept
    {
    }

    StatelessActor::StatelessActor() noexcept
        : impl_(std::make_unique<impl_type>())
    {
    }

    StatelessActor::StatelessActor(StatelessActor &&other) noexcept
        : impl_(std::move(other.impl_))
    {
    }

    auto StatelessActor::define_writer(const String &name,
                                       const Function &function) const noexcept -> void
    {
        impl_->define(name, impl_type::concurrency_type::EXCLUSIVE, function);
    }

    auto StatelessActor::define_reader(const String &name,
                                       const Function &function) const noexcept -> void
    {
        impl_->define(name, impl_type::concurrency_type::SHARED, function);
    }

    auto StatelessActor::mailbox() const noexcept -> Mailbox
    {
        return Mailbox{impl_->mailbox()};
    }

    auto StatelessActor::mailbox_interface() const noexcept -> std::unique_ptr<Mailbox::Interface>
    {
        return impl_->mailbox();
    }
}
