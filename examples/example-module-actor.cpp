// SPDX-License-Identifier: BSL-1.0

#include <thread>
#include <chrono>
#include <iostream>

#include <traeger/module/Module.hpp>

extern void perform_operations(const traeger::Scheduler &scheduler, const traeger::Mailbox &mailbox);

int main(const int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing argument" << std::endl;
        return 1;
    }

    const traeger::String path = argv[1];
    std::cout << "Attempting to load module from path: " << path << std::endl;

    auto [module_optional, module_error] = traeger::Module::from_path(path,
                                                                      traeger::make_map("initial_funds", 1000.0));

    if (!module_optional)
    {
        std::cout << "Module error: " << module_error << std::endl;
        return 1;
    }
    const auto loaded_module = module_optional.value();
    const auto mailbox = loaded_module.mailbox();
    const auto scheduler = traeger::Scheduler{traeger::Threads{8}};

    perform_operations(scheduler, mailbox);

    while (scheduler.count() != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
