// SPDX-License-Identifier: BSL-1.0

#include <variant>
#include <stdexcept>
#include <utility>

#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>

#include <traeger/value/Variant.hpp>
#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>

#include "traeger_python.hpp"

namespace
{
    namespace nb = nanobind;
    using namespace traeger;

    using string_variant = std::variant<String, nb::bytes>;

    template <class... Types>
    struct overload : Types...
    {
        using Types::operator()...;
    };

    template <class... Types>
    overload(Types...) -> overload<Types...>;

    auto format_init(Format *self,
                     String name) -> void
    {
        const auto *result = Format::by_name(name);
        if (result != nullptr)
        {
            new (self) Format{*result};
        }
        else
        {
            throw nb::value_error("undefined Format");
        }
    }

    auto result_to_string_or_bytes(String &&content,
                                   const String &format_name) -> string_variant
    {
        if (format_name == "msgpack")
        {
            return nb::bytes{content.data(), content.size()};
        }
        else
        {
            return std::move(content);
        }
    }

    auto format_encode_value(Format &self,
                             const Value &value) -> string_variant
    {
        auto [result, error] = self.encode(value);
        if (result)
        {
            return result_to_string_or_bytes(std::move(result).value(), self.name());
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto format_encode_variant(Format &self,
                               const Variant &variant) -> string_variant
    {
        auto [result, error] = self.encode(value_from_variant(variant));
        if (result)
        {
            return result_to_string_or_bytes(std::move(result).value(), self.name());
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto format_decode(Format &self,
                       const nb::object &object) -> Value
    {
        auto content = std::visit(
            overload{
                [](nb::bytes bytes)
                { return String{bytes.c_str(), bytes.size()}; },
                [](String string)
                { return string; }},
            nb::cast<string_variant>(object));

        auto [result, error] = self.decode(content);
        if (result)
        {
            return std::move(result).value();
        }
        else
        {
            throw std::runtime_error(error);
        }
    }

    auto format_repr(const Format &self) -> String
    {
        return "traeger.Format(\"" + self.name() + "\")";
    }
}

auto traeger_python_register_format_types(nb::module_ &module) -> void
{
    auto format_class = nb::class_<Format>(module, "Format");

    format_class
        .def("__init__", &format_init)
        .def("name", &Format::name)
        .def("encode", &format_encode_value)
        .def("encode", &format_encode_variant, nb::arg("value").none())
        .def("decode", &format_decode)
        .def("__repr__", &format_repr)
#if defined(TRAEGER_FORMAT_JSON)
        .def_static("json", &Format::json)
#endif
#if defined(TRAEGER_FORMAT_YAML)
        .def_static("yaml", &Format::yaml)
#endif
#if defined(TRAEGER_FORMAT_MSGPACK)
        .def_static("msgpack", &Format::msgpack)
#endif
        ;
}
