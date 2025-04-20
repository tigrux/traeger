// SPDX-License-Identifier: BSL-1.0

#include <array>
#include <array>
#include <exception>
#include <optional>
#include <string_view>
#include <utility>

#include "traeger/value/Value.hpp"
#include "traeger/format/Format.hpp"

extern "C"
{
#if defined(TRAEGER_FORMAT_JSON)
    extern traeger_format_t traeger_format_json;
#endif
#if defined(TRAEGER_FORMAT_YAML)
    extern traeger_format_t traeger_format_yaml;
#endif
#if defined(TRAEGER_FORMAT_MSGPACK)
    extern traeger_format_t traeger_format_msgpack;
#endif
}

namespace
{
    const std::array traeger_known_formats{
#if defined(TRAEGER_FORMAT_JSON)
        &traeger_format_json,
#endif
#if defined(TRAEGER_FORMAT_YAML)
        &traeger_format_yaml,
#endif
#if defined(TRAEGER_FORMAT_MSGPACK)
        &traeger_format_msgpack,
#endif
    };
}

namespace traeger
{
    Format::Format(const String &name,
                   encoder_type *encoder,
                   decoder_type *decoder) noexcept
        : name_(name),
          encoder_(encoder),
          decoder_(decoder)
    {
    }

    auto Format::name() const noexcept -> const String &
    {
        return name_;
    }

    auto Format::encode(const Value &value) const noexcept -> std::pair<std::optional<String>, String>
    {
        if (encoder_ == nullptr)
        {
            return {std::nullopt, "Format encoder is null"};
        }
        try
        {
            return {encoder_(value), String{}};
        }
        catch (const std::exception &e)
        {
            return {std::nullopt, e.what()};
        }
    }

    auto Format::decode(const std::string_view &content) const noexcept -> std::pair<std::optional<Value>, String>
    {
        if (decoder_ == nullptr)
        {
            return {std::nullopt, "Format decoder is null"};
        }
        try
        {
            return {decoder_(content), String{}};
        }
        catch (const std::exception &e)
        {
            return {std::nullopt, e.what()};
        }
    }

    auto Format::by_name(const String &name) noexcept -> const traeger_format_t *
    {
        for (const auto *format : traeger_known_formats)
        {
            if (format->name() == name)
            {
                return format;
            }
        }
        return nullptr;
    }

#if defined(TRAEGER_FORMAT_JSON)
    auto Format::json() noexcept -> const Format &
    {
        return traeger_format_json;
    }
#endif
#if defined(TRAEGER_FORMAT_YAML)
    auto Format::yaml() noexcept -> const Format &
    {
        return traeger_format_yaml;
    }
#endif
#if defined(TRAEGER_FORMAT_MSGPACK)
    auto Format::msgpack() noexcept -> const Format &
    {
        return traeger_format_msgpack;
    }
#endif
}
