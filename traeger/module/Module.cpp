// SPDX-License-Identifier: BSL-1.0

#include <memory>

#include <traeger/module/Module.hpp>

#ifdef _WIN32
#include <windows.h>

namespace
{
    void module_close(HMODULE module_ptr) noexcept
    {
        FreeLibrary(module_ptr);
    }

    using module_ptr_t = std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&module_close)>;

    auto module_open(const char *path, std::string *error, ...) noexcept -> module_ptr_t
    {
        auto module_ptr = module_ptr_t{LoadLibraryA(path), &module_close};
        if (!module_ptr)
        {
            auto last_error = GetLastError();
            char *formatted_error = nullptr;
            va_list ap;
            va_start(ap, error);
            if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                               nullptr,
                               last_error,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (char *)&formatted_error,
                               0,
                               &ap))
            {
                *error = formatted_error;
                LocalFree(formatted_error);
            }
            else
            {
                *error = "failed to format error";
            }
            va_end(ap);
        }
        return module_ptr;
    }

    auto module_get_symbol(const module_ptr_t &module_ptr, const char *symbol) noexcept -> traeger_module_init_t
    {
        return reinterpret_cast<traeger_module_init_t>(GetProcAddress(module_ptr.get(), symbol));
    }
}

#else
#include <dlfcn.h>

namespace
{
    void module_close(void *module_ptr) noexcept
    {
        dlclose(module_ptr);
    }

    using module_ptr_t = std::unique_ptr<void, decltype(&module_close)>;

    auto module_open(const char *path, std::string *error, ...) noexcept -> module_ptr_t
    {
        auto module_ptr = module_ptr_t{dlopen(path, RTLD_LAZY), &module_close};
        if (!module_ptr)
        {
            *error = dlerror();
        }
        return module_ptr;
    }

    auto module_get_symbol(const module_ptr_t &module_ptr, const char *symbol) noexcept -> traeger_module_init_t
    {
        return reinterpret_cast<traeger_module_init_t>(dlsym(module_ptr.get(), symbol));
    }
}

#endif

namespace traeger
{
    struct Module::impl_type : public Mailbox::Interface
    {
        impl_type(module_ptr_t &&module_ptr,
                  std::shared_ptr<Mailbox::Interface> &&mailbox_interface) noexcept
            : module_ptr_(std::move(module_ptr)),
              mailbox_(std::move(mailbox_interface))
        {
        }

        Promise
        send(const Scheduler &scheduler,
             const String &name,
             const List &arguments) noexcept override
        {
            return mailbox_.send(scheduler, name, arguments);
        }

    private:
        module_ptr_t module_ptr_;
        Mailbox mailbox_;
    };

    Module::~Module() noexcept
    {
    }

    Module::Module(std::shared_ptr<impl_type> &&impl) noexcept
        : impl_(std::move(impl))
    {
    }

    auto Module::mailbox() const noexcept -> Mailbox
    {
        return Mailbox{impl_};
    }

    auto Module::from_path(const String &path,
                           const Map &configuration) noexcept -> std::pair<std::optional<Module>, String>
    {
        std::string module_error;
        auto module_ptr = module_open(path.c_str(), &module_error, path.c_str());
        if (!module_ptr)
        {
            return {std::nullopt, module_error};
        }

        const char *init_symbol = "traeger_module_init";
        auto module_init = module_get_symbol(module_ptr, init_symbol);
        if (!module_init)
        {
            return {std::nullopt, "failed to find initial symbol"};
        }

        const traeger_map_t map{configuration};
        traeger_mailbox_interface_t *mailbox_interface = nullptr;
        traeger_string_t error{};
        module_init(&map, &mailbox_interface, &error);
        if (!mailbox_interface)
        {
            return {std::nullopt, "failed to initializing module: " + error};
        }

        return {
            Module{
                std::make_shared<impl_type>(
                    std::move(module_ptr),
                    std::shared_ptr<Mailbox::Interface>(mailbox_interface)),
            },
            String{},
        };
    }
}
