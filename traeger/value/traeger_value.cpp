// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <sstream>

#include <stdlib.h>

#include "traeger/value/traeger_value.hpp"

namespace
{
    template <typename T>
    traeger_string_t *to_string(const T &self) noexcept
    {
        std::ostringstream ss;
        ss << self;
        return new traeger_string_t{std::move(ss).str()};
    }
}

extern "C"
{
    using namespace traeger;

    // String

    traeger_string_t *traeger_string_new(const char *string_data,
                                         size_t string_size)
    {
        if (string_data != nullptr)
        {
            return new traeger_string_t{String(string_data, string_size)};
        }
        return nullptr;
    }

    void traeger_string_free(traeger_string_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    const char *traeger_string_data(const traeger_string_t *self)
    {
        return cast(self).data();
    }

    size_t traeger_string_size(const traeger_string_t *self)
    {
        return cast(self).size();
    }

    // List

    traeger_list_t *traeger_list_new()
    {
        return new traeger_list_t{};
    }

    traeger_list_t *traeger_list_copy(const traeger_list_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_list_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_list_free(traeger_list_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    void traeger_list_print(const traeger_list_t *self)
    {
        if (self != nullptr)
        {
            std::cout << cast(self) << std::endl;
        }
    }

    void traeger_list_to_string(const traeger_list_t *self,
                                traeger_string_t **string)
    {
        if (self != nullptr &&
            string != nullptr)
        {
            *string = to_string(cast(self));
        }
    }

    void traeger_list_append_value(traeger_list_t *self,
                                   const traeger_value_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            cast(self).append(cast(value));
        }
    }

    void traeger_list_append_null(traeger_list_t *self,
                                  void *)
    {
        if (self != nullptr)
        {
            cast(self).append(nullptr);
        }
    }

    void traeger_list_append_bool(traeger_list_t *self,
                                  traeger_bool_t value)
    {
        if (self != nullptr)
        {
            cast(self).append(value);
        }
    }

    void traeger_list_append_int(traeger_list_t *self,
                                 traeger_int_t value)
    {
        if (self != nullptr)
        {
            cast(self).append(value);
        }
    }

    void traeger_list_append_uint(traeger_list_t *self,
                                  traeger_uint_t value)
    {
        if (self != nullptr)
        {
            cast(self).append(value);
        }
    }

    void traeger_list_append_float(traeger_list_t *self,
                                   traeger_float_t value)
    {
        if (self != nullptr)
        {
            cast(self).append(value);
        }
    }

    void traeger_list_append_string(traeger_list_t *self,
                                    const char *string_data,
                                    size_t string_size)
    {
        if (self != nullptr &&
            string_data != nullptr)
        {
            cast(self).append(String(string_data, string_size));
        }
    }

    void traeger_list_append_list(traeger_list_t *self,
                                  const traeger_list_t *list)
    {
        if (self != nullptr &&
            list != nullptr)
        {
            cast(self).append(cast(list));
        }
    }

    void traeger_list_append_map(traeger_list_t *self,
                                 const traeger_map_t *map)
    {
        if (self != nullptr &&
            map != nullptr)
        {
            cast(self).append(cast(map));
        }
    }

    void traeger_list_set_value(traeger_list_t *self,
                                int index,
                                const traeger_value_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            cast(self).set(index, cast(value));
        }
    }

    void traeger_list_set_null(traeger_list_t *self,
                               int index,
                               void *)
    {
        if (self != nullptr)
        {
            cast(self).set(index, nullptr);
        }
    }

    void traeger_list_set_bool(traeger_list_t *self,
                               int index,
                               traeger_bool_t value)
    {
        if (self != nullptr)
        {
            cast(self).set(index, value);
        }
    }

    void traeger_list_set_int(traeger_list_t *self,
                              int index,
                              traeger_int_t value)
    {
        if (self != nullptr)
        {
            cast(self).set(index, value);
        }
    }

    void traeger_list_set_uint(traeger_list_t *self,
                               int index,
                               traeger_uint_t value)
    {
        if (self != nullptr)
        {
            cast(self).set(index, value);
        }
    }

    void traeger_list_set_float(traeger_list_t *self,
                                int index,
                                traeger_float_t value)
    {
        if (self != nullptr)
        {
            cast(self).set(index, value);
        }
    }

    void traeger_list_set_string(traeger_list_t *self,
                                 int index,
                                 const char *string_data,
                                 size_t string_size)

    {
        if (self != nullptr &&
            string_data != nullptr)
        {
            cast(self).set(index, String(string_data, string_size));
        }
    }

    void traeger_list_set_list(traeger_list_t *self,
                               int index,
                               const traeger_list_t *list)

    {
        if (self != nullptr &&
            list != nullptr)
        {
            cast(self).set(index, cast(list));
        }
    }

    void traeger_list_set_map(traeger_list_t *self,
                              int index,
                              const traeger_map_t *map)

    {
        if (self != nullptr &&
            map != nullptr)
        {
            cast(self).set(index, cast(map));
        }
    }

    bool traeger_list_find(const traeger_list_t *self,
                           int index,
                           traeger_value_t **value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            if (const auto *ptr_value = cast(self).find(index); ptr_value)
            {
                *value = new traeger_value_t{*ptr_value};
                return true;
            }
        }
        return false;
    }

    bool traeger_list_empty(const traeger_list_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).empty();
        }
        return false;
    }

    size_t traeger_list_size(const traeger_list_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).size();
        }
        return 0;
    }

    size_t traeger_list_resize(traeger_list_t *self,
                               size_t new_size)
    {
        if (self != nullptr)
        {
            return cast(self).resize(new_size);
        }
        return 0;
    }

    // List::Iterator

    traeger_list_iterator_t *traeger_list_iterator_new(const traeger_list_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_list_iterator_t{cast(self).begin()};
        }
        return nullptr;
    }

    void traeger_list_iterator_free(traeger_list_iterator_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    bool traeger_list_iterator_has_next(traeger_list_iterator_t *self)
    {
        if (self != nullptr)
        {
            if (cast(self))
            {
                return true;
            }
        }
        return false;
    }

    bool traeger_list_iterator_next(traeger_list_iterator_t *self,
                                    traeger_value_t **value)
    {
        if (self != nullptr)
        {
            if (cast(self))
            {
                if (value != nullptr)
                {
                    *value = new traeger_value_t{cast(self).value()};
                }
                cast(self).increment();
                return true;
            }
        }
        return false;
    }

    // Map

    traeger_map_t *traeger_map_new()
    {
        return new traeger_map_t{};
    }

    traeger_map_t *traeger_map_copy(const traeger_map_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_map_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_map_free(traeger_map_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    void traeger_map_print(const traeger_map_t *self)
    {
        if (self != nullptr)
        {
            std::cout << cast(self) << std::endl;
        }
    }

    void traeger_map_to_string(const traeger_map_t *self,
                               traeger_string_t **string)
    {
        if (self != nullptr &&
            string != nullptr)
        {
            *string = to_string(cast(self));
        }
    }

    // Map::set

    void traeger_map_set_value(traeger_map_t *self,
                               const char *key_data,
                               size_t key_size,
                               const traeger_value_t *value)
    {
        if (self != nullptr &&
            key_data != nullptr &&
            value != nullptr)
        {
            cast(self).set(String(key_data, key_size), cast(value));
        }
    }

    void traeger_map_set_null(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              void *)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), nullptr);
        }
    }

    void traeger_map_set_bool(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              traeger_bool_t value)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), value);
        }
    }

    void traeger_map_set_int(traeger_map_t *self,
                             const char *key_data,
                             size_t key_size,
                             traeger_int_t value)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), value);
        }
    }

    void traeger_map_set_uint(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              traeger_uint_t value)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), value);
        }
    }

    void traeger_map_set_float(traeger_map_t *self,
                               const char *key_data,
                               size_t key_size,
                               traeger_float_t value)

    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), value);
        }
    }

    void traeger_map_set_string(traeger_map_t *self,
                                const char *key_data,
                                size_t key_size,
                                const char *string_data,
                                size_t string_size)
    {
        if (self != nullptr &&
            key_data != nullptr &&
            string_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), String(string_data, string_size));
        }
    }

    void traeger_map_set_list(traeger_map_t *self,
                              const char *key_data,
                              size_t key_size,
                              const traeger_list_t *list)

    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).set(String(key_data, key_size), cast(list));
        }
    }

    void traeger_map_set_map(traeger_map_t *self,
                             const char *key_data,
                             size_t key_size,
                             const traeger_map_t *map)
    {
        if (self != nullptr &&
            key_data != nullptr &&
            map != nullptr)
        {
            cast(self).set(String(key_data, key_size), cast(map));
        }
    }

    void traeger_map_erase(traeger_map_t *self,
                           const char *key_data,
                           size_t key_size)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            cast(self).erase(String(key_data, key_size));
        }
    }

    bool traeger_map_contains(const traeger_map_t *self,
                              const char *key_data,
                              size_t key_size)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            return cast(self).contains(String(key_data, key_size));
        }
        return false;
    }

    bool traeger_map_find(const traeger_map_t *self,
                          const char *key_data,
                          size_t key_size,
                          traeger_value_t **value)
    {
        if (self != nullptr &&
            key_data != nullptr)
        {
            if (const auto *ptr_value = cast(self).find(String(key_data, key_size)); ptr_value)
            {
                *value = new traeger_value_t{*ptr_value};
                return true;
            }
        }
        return false;
    }

    bool traeger_map_empty(const traeger_map_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).empty();
        }
        return false;
    }

    size_t traeger_map_size(const traeger_map_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).size();
        }
        return 0;
    }

    // Map::Iterator

    traeger_map_iterator_t *traeger_map_iterator_new(const traeger_map_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_map_iterator_t{cast(self).begin()};
        }
        return nullptr;
    }

    void traeger_map_iterator_free(traeger_map_iterator_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    bool traeger_map_iterator_has_next(traeger_map_iterator_t *self)
    {
        if (self != nullptr)
        {
            if (cast(self))
            {
                return true;
            }
        }
        return false;
    }

    bool traeger_map_iterator_next(traeger_map_iterator_t *self,
                                   traeger_string_t **key,
                                   traeger_value_t **value)
    {
        if (self != nullptr)
        {
            if (cast(self))
            {
                if (value != nullptr)
                {
                    *value = new traeger_value_t{cast(self).value()};
                }
                if (key != nullptr)
                {
                    *key = new traeger_string_t{cast(self).key()};
                }
                cast(self).increment();
                return true;
            }
        }
        return false;
    }

    traeger_value_t *traeger_value_new()
    {
        return new traeger_value_t{};
    }

    traeger_value_t *traeger_value_copy(const traeger_value_t *self)
    {
        if (self != nullptr)
        {
            return new traeger_value_t{cast(self)};
        }
        return nullptr;
    }

    void traeger_value_free(traeger_value_t *self)
    {
        if (self != nullptr)
        {
            delete self;
        }
    }

    void traeger_value_print(const traeger_value_t *self)
    {
        if (self != nullptr)
        {
            std::cout << cast(self) << std::endl;
        }
    }

    void traeger_value_to_string(const traeger_value_t *self,
                                 traeger_string_t **string)
    {
        if (self != nullptr &&
            string != nullptr)
        {
            *string = to_string(cast(self));
        }
    }

    traeger_value_type_t traeger_value_get_type(const traeger_value_t *self)
    {
        if (self != nullptr)
        {
            return cast(cast(self).type());
        }
        return TRAEGER_VALUE_TYPE_NULL;
    }

    const char *traeger_value_get_type_name(const traeger_value_t *self)
    {
        if (self != nullptr)
        {
            return cast(self).type_name().c_str();
        }
        return nullptr;
    }

    const char *traeger_value_type_get_name(traeger_value_type_t type)
    {
        const auto value_type = Value::Type(static_cast<int>(type));
        return Value::type_name(value_type).c_str();
    }

    void traeger_value_set_null(traeger_value_t *self, void *)
    {
        if (self != nullptr)
        {
            cast(self) = nullptr;
        }
    }

    void traeger_value_set_bool(traeger_value_t *self,
                                traeger_bool_t value)
    {
        if (self != nullptr)
        {
            cast(self) = value;
        }
    }

    void traeger_value_set_int(traeger_value_t *self,
                               traeger_int_t value)
    {
        if (self != nullptr)
        {
            cast(self) = value;
        }
    }

    void traeger_value_set_uint(traeger_value_t *self,
                                traeger_uint_t value)
    {
        if (self != nullptr)
        {
            cast(self) = value;
        }
    }

    void traeger_value_set_float(traeger_value_t *self,
                                 traeger_float_t value)
    {
        if (self != nullptr)
        {
            cast(self) = value;
        }
    }

    void traeger_value_set_string(traeger_value_t *self,
                                  const char *string_data,
                                  size_t string_size)
    {
        if (self != nullptr)
        {
            cast(self) = String(string_data, string_size);
        }
    }

    void traeger_value_set_list(traeger_value_t *self,
                                const traeger_list_t *list)
    {
        if (self != nullptr &&
            list != nullptr)
        {
            cast(self) = *list;
        }
    }

    void traeger_value_set_map(traeger_value_t *self,
                               const traeger_map_t *map)
    {
        if (self != nullptr &&
            map != nullptr)
        {
            cast(self) = *map;
        }
    }

    void traeger_value_set_value(traeger_value_t *self,
                                 const traeger_value_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            cast(self) = cast(value);
        }
    }

    bool traeger_value_get_null(const traeger_value_t *self,
                                void *)
    {
        if (self != nullptr)
        {
            const auto optional = cast(self).get_null();
            if (optional)
            {
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_bool(const traeger_value_t *self,
                                traeger_bool_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            const auto optional = cast(self).get_bool();
            if (optional)
            {
                *value = optional.value();
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_int(const traeger_value_t *self,
                               traeger_int_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            const auto optional = cast(self).get_int();
            if (optional)
            {
                *value = optional.value();
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_uint(const traeger_value_t *self,
                                traeger_uint_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            const auto optional = cast(self).get_uint();
            if (optional)
            {
                *value = optional.value();
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_float(const traeger_value_t *self,
                                 traeger_float_t *value)
    {
        if (self != nullptr &&
            value != nullptr)
        {
            const auto optional = cast(self).get_float();
            if (optional)
            {
                *value = optional.value();
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_string(const traeger_value_t *self,
                                  traeger_string_t **string)
    {
        if (self != nullptr &&
            string != nullptr)
        {
            const auto optional = cast(self).get_string();
            if (optional)
            {
                *string = new traeger_string_t{*optional};
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_list(const traeger_value_t *self,
                                traeger_list_t **list)
    {
        if (self != nullptr &&
            list != nullptr)
        {
            const auto optional = cast(self).get_list();
            if (optional)
            {
                *list = new traeger_list_t{std::move(optional).value()};
                return true;
            }
        }
        return false;
    }

    bool traeger_value_get_map(const traeger_value_t *self,
                               traeger_map_t **map)
    {
        if (self != nullptr &&
            map != nullptr)
        {
            const auto optional = cast(self).get_map();
            if (optional)
            {
                *map = new traeger_map_t{std::move(optional).value()};
                return true;
            }
        }
        return false;
    }
}
