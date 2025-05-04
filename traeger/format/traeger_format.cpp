// SPDX-License-Identifier: BSL-1.0

#include "traeger/value/traeger_value.hpp"
#include "traeger/format/traeger_format.hpp"

extern "C"
{
    using namespace traeger;

    const traeger_format_t *traeger_format_get_by_name(const char *name_data,
                                                       size_t name_size)
    {
        if (name_data != nullptr)
        {
            return Format::by_name(String(name_data, name_size));
        }
        return nullptr;
    }

    const traeger_string_t *traeger_format_get_name(const traeger_format_t *self)
    {
        if (self != nullptr)
        {
            return &cast(self).name();
        }
        return nullptr;
    }

    bool traeger_format_encode_value(const traeger_format_t *self,
                                     const traeger_value_t *value,
                                     traeger_string_t **result,
                                     traeger_string_t **error)
    {
        if (value != nullptr &&
            result != nullptr)
        {
            auto [encode_result, encode_error] = cast(self).encode(cast(value));
            if (encode_result)
            {
                *result = new traeger_string_t{std::move(encode_result).value()};
                return true;
            }
            else if (error)
            {
                *error = new traeger_string_t{std::move(encode_error)};
            }
        }
        return false;
    }

    bool traeger_format_decode_string(const traeger_format_t *self,
                                      const traeger_string_t *string,
                                      traeger_value_t **result,
                                      traeger_string_t **error)
    {
        if (self != nullptr &&
            string != nullptr &&
            result != nullptr)
        {
            auto [decode_result, decode_error] = cast(self).decode(cast(string));
            if (decode_result)
            {
                *result = new traeger_value_t{std::move(decode_result).value()};
                return true;
            }
            else if (error)
            {
                *error = new traeger_string_t{std::move(decode_error)};
            }
        }
        return false;
    }
}
