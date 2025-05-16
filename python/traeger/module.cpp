// SPDX-License-Identifier: BSL-1.0

#include <nanobind/stl/string.h>

#include <traeger/module/Module.hpp>

#include "traeger.hpp"

namespace
{
    namespace nb = nanobind;
    using namespace traeger;

    auto module_init(Module *self,
                     String path,
                     Map map) -> void
    {
        auto [module_optional, module_error] = Module::from_path(path, map);
        if (module_optional)
        {
            new (self) Module{std::move(module_optional).value()};
        }
        else
        {
            throw std::runtime_error(module_error);
        }
    }
}

auto traeger_python_register_module_types(nb::module_ &module) -> void
{
    auto module_class = nb::class_<Module>(module, "Module");

    module_class
        .def("__init__", &module_init)
        .def("mailbox", &Module::mailbox);
}