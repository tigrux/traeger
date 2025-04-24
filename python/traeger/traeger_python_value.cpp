// SPDX-License-Identifier: BSL-1.0

#include <utility>

#include <nanobind/stl/string.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/pair.h>

#include <traeger/value/Variant.hpp>
#include <traeger/value/Value.hpp>

#include "traeger_python.hpp"

namespace
{
    namespace nb = nanobind;
    using namespace traeger;

    template <typename Getter>
    struct MapIterator
    {
        Map::Iterator iterator;

        auto iter()
        {
            return *this;
        }

        auto next()
        {
            if (iterator)
            {
                auto result = Getter::get(iterator);
                iterator.increment();
                return result;
            }
            throw nb::stop_iteration();
        }

        static const inline Getter get;
    };

    namespace MapIteratorGet
    {
        struct Keys
        {
            static auto get(const Map::Iterator &iterator)
            {
                return iterator.key();
            }
        };

        struct Values
        {
            static auto get(const Map::Iterator &iterator)
            {
                return iterator.value();
            }
        };

        struct Items
        {
            static auto get(const Map::Iterator &iterator)
            {
                return *iterator;
            }
        };
    }

    using MapKeyIterator = MapIterator<MapIteratorGet::Keys>;

    using MapValueIterator = MapIterator<MapIteratorGet::Values>;

    using MapItemIterator = MapIterator<MapIteratorGet::Items>;

    auto list_from_sequence(nb::sequence sequence) -> List
    {
        List result;
        Variant variant;
        Value value;
        auto throw_error = []
        {
            throw nb::type_error(
                "argument must be a Sequence[None | bool | int | float | str | traeger.List | traeger.Map]");
        };
        for (auto handle : sequence)
        {
            if (nb::try_cast(handle, variant))
            {
                result.append(value_from_variant(std::move(variant)));
            }
            else if (nb::try_cast(handle, value))
            {
                result.append(std::move(value));
            }
            else
            {
                throw_error();
            }
        }
        return result;
    }

    auto list_init(List *self,
                   nb::sequence sequence) -> void
    {
        new (self) List{list_from_sequence(sequence)};
    }

    auto list_append_value(List &self,
                           const Value &value) -> void
    {
        self.append(value);
    }

    auto list_append_variant(List &self,
                             const Variant &variant) -> void
    {
        self.append(value_from_variant(variant));
    }

    auto map_from_mapping(nb::mapping mapping) -> Map
    {
        Map result;
        String key;
        Variant variant;
        Value value;
        nb::handle value_handle;
        auto throw_error = []
        {
            throw nb::type_error("argument must be a Mapping[str, None | bool | int | float | str | traeger.List | traeger.Map]");
        };
        for (auto key_handle : mapping)
        {
            if (!nb::try_cast(key_handle, key))
            {
                throw_error();
            }
            value_handle = mapping[key_handle];
            if (nb::try_cast(value_handle, variant))
            {
                result.set(std::move(key), value_from_variant(std::move(variant)));
            }
            else if (nb::try_cast(value_handle, value))
            {
                result.set(std::move(key), std::move(value));
            }
            else
            {
                throw_error();
            }
        }
        return result;
    }

    auto map_init(Map *self,
                  nb::mapping mapping) -> void
    {
        new (self) Map{map_from_mapping(mapping)};
    }

    auto list_repr(const List &self) -> String
    {
        return type_repr("traeger.List", self);
    }

    auto list_find(List &self,
                   Int index) -> Value
    {
        if (const auto *ptr_value = self.find(index); ptr_value)
        {
            return *ptr_value;
        }
        throw nb::index_error("index out of range");
    }

    auto list_set_value(List &self,
                        Int index,
                        const Value &value) -> void
    {
        if (!self.set(index, value))
        {
            throw nb::index_error("index out of range");
        }
    }

    auto list_set_variant(List &self,
                          Int index,
                          const Variant &variant) -> void
    {
        if (!self.set(index, value_from_variant(variant)))
        {
            throw nb::index_error("index out of range");
        }
    }

    auto list_iter(const List &self) -> List::Iterator
    {
        return self.begin();
    }

    auto list_iterator_next(List::Iterator &self) -> Value
    {
        if (self)
        {
            auto value = self.value();
            self.increment();
            return value;
        }
        throw nb::stop_iteration();
    }

    auto map_repr(const Map &self) -> String
    {
        return type_repr("traeger.Map", self);
    }

    auto map_find(Map &self,
                  const String &key) -> Value
    {
        if (const auto *ptr_value = self.find(key); ptr_value)
        {
            return *ptr_value;
        }
        throw nb::key_error("key not found");
    }

    auto map_set_value(Map &self,
                       const String &key,
                       const Value &value) -> void
    {
        self.set(key, value);
    }

    auto map_set_variant(Map &self,
                         const String &key,
                         const Variant &variant) -> void
    {
        self.set(key, value_from_variant(variant));
    }

    auto map_key_iter(const Map &self)
    {
        return MapKeyIterator{self.begin()};
    }

    auto map_value_iter(const Map &self)
    {
        return MapValueIterator{self.begin()};
    }

    auto map_item_iter(const Map &self)
    {
        return MapItemIterator{self.begin()};
    }

    auto value_init_from_variant(Value *self,
                                 const Variant &variant) -> void
    {
        new (self) Value{value_from_variant(variant)};
    }

    auto value_init_from_sequence(Value *self,
                                  nb::sequence sequence) -> void
    {
        new (self) Value{List{list_from_sequence(sequence)}};
    }

    auto value_init_from_mapping(Value *self,
                                 nb::mapping mapping) -> void
    {
        new (self) Value{Map{map_from_mapping(mapping)}};
    }

    auto value_type(const Value &self)
    {
        return self.type();
    }

    auto value_type_name(const Value &self)
    {
        return self.type_name();
    }

    auto value_get_variant(const Value &self) -> Variant
    {
        return value_to_variant(self);
    }

    auto value_set_variant(Value &self,
                           const Variant &variant) -> void
    {
        self = value_from_variant(variant);
    }

    auto value_repr(const Value &self)
    {
        return type_repr("traeger.Value", self);
    }

    auto value_type_repr(Value::Type self)
    {
        return "traeger.Value.Type." + Value::type_name(self);
    }
}

auto traeger_python_register_value_types(nb::module_ &module) -> void
{
    auto list_class = nb::class_<List>(module, "List");
    auto list_iterator_class = nb::class_<List::Iterator>(module, "List_iterator");
    auto map_class = nb::class_<Map>(module, "Map");
    auto map_key_iterator_class = nb::class_<MapKeyIterator>(module, "Map_key_iterator");
    auto map_value_iterator_class = nb::class_<MapValueIterator>(module, "Map_value_iterator");
    auto map_item_iterator_class = nb::class_<MapItemIterator>(module, "Map_item_iterator");
    auto value_class = nb::class_<Value>(module, "Value");
    auto value_type_enum = nb::enum_<Value::Type>(value_class, "Type");

    list_class
        .def(nb::init<>())
        .def("__init__", &list_init)
        .def("append", &list_append_value)
        .def("append", &list_append_variant, nb::arg("value").none())
        .def("copy", &type_copy<List>)
        .def("resize", &List::resize)
        .def("__repr__", &list_repr)
        .def("__str__", &type_str<List>)
        .def("__eq__", &List::operator==)
        .def("__ne__", &List::operator!=)
        .def("__len__", &List::size)
        .def("__bool__", &type_bool<List>)
        .def("__getitem__", &list_find)
        .def("__setitem__", &list_set_value)
        .def("__setitem__", &list_set_variant, nb::arg("index"), nb::arg("value").none())
        .def("__iter__", &list_iter);

    list_iterator_class
        .def("__next__", &list_iterator_next);

    map_class
        .def(nb::init<>())
        .def("__init__", &map_init)
        .def("copy", &type_copy<Map>)
        .def("__repr__", &map_repr)
        .def("__str__", &type_str<Map>)
        .def("__eq__", &Map::operator==)
        .def("__ne__", &Map::operator!=)
        .def("__len__", &Map::size)
        .def("__bool__", &type_bool<Map>)
        .def("__contains__", &Map::contains)
        .def("__getitem__", &map_find)
        .def("__setitem__", &map_set_value)
        .def("__setitem__", &map_set_variant, nb::arg("key"), nb::arg("value").none())
        .def("__delitem__", &Map::erase)
        .def("__iter__", &map_key_iter)
        .def("keys", &map_key_iter)
        .def("values", &map_value_iter)
        .def("items", &map_item_iter);

    map_key_iterator_class
        .def("__iter__", &MapKeyIterator::iter)
        .def("__next__", &MapKeyIterator::next);

    map_value_iterator_class
        .def("__iter__", &MapValueIterator::iter)
        .def("__next__", &MapValueIterator::next);

    map_item_iterator_class
        .def("__iter__", &MapItemIterator::iter)
        .def("__next__", &MapItemIterator::next);

    value_class
        .def(nb::init<>())
        .def("__init__", &value_init_from_variant, nb::arg("value").none())
        .def("__init__", &value_init_from_sequence)
        .def("__init__", &value_init_from_mapping)
        .def("type", &value_type)
        .def("type_name", &value_type_name)
        .def("get", &value_get_variant)
        .def("set", &value_set_variant, nb::arg("value").none())
        .def("copy", &type_copy<Value>)
        .def("__repr__", &value_repr)
        .def("__str__", &type_str<Value>)
        .def("__eq__", &Value::operator==)
        .def("__ne__", &Value::operator!=);

    value_type_enum
        .value("Null", Value::Type::Null)
        .value("Bool", Value::Type::Bool)
        .value("Int", Value::Type::Int)
        .value("UInt", Value::Type::UInt)
        .value("Float", Value::Type::Float)
        .value("String", Value::Type::String)
        .value("List", Value::Type::List)
        .value("Map", Value::Type::Map)
        .def("__repr__", &value_type_repr);
}
