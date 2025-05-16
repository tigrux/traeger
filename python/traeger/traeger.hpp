// SPDX-License-Identifier: BSL-1.0

#include <sstream>

#include <nanobind/nanobind.h>

namespace
{
    namespace nb = nanobind;

    template <typename Type>
    auto type_copy(const Type &self)
    {
        return self;
    }

    template <typename Type>
    auto type_str(const Type &self)
    {
        std::stringstream ss;
        ss << self;
        return ss.str();
    }

    template <typename Type>
    auto type_repr(const char *type_name,
                   const Type &self)
    {
        std::stringstream ss;
        ss << "<" << type_name << "(" << self << ") at " << &self << '>';
        return ss.str();
    }

    template <typename Type>
    auto type_bool(const Type &self) -> bool
    {
        return self.empty() == false;
    }
}

auto traeger_python_register_value_types(nb::module_ &module) -> void;

auto traeger_python_register_format_types(nb::module_ &module) -> void;

auto traeger_python_register_actor_types(nb::module_ &module) -> void;

auto traeger_python_register_socket_types(nb::module_ &module) -> void;

auto traeger_python_register_module_types(nb::module_ &module) -> void;
