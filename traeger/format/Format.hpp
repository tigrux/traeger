// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <string_view>
#include <optional>

#include <traeger/value/Types.hpp>

struct traeger_format_t;

namespace traeger
{
    struct Format
    {
        using encoder_type = auto(const Value &value) -> String;

        using decoder_type = auto(const std::string_view &content) -> Value;

        Format() = delete;

        Format(const String &name,
               encoder_type *encoder,
               decoder_type *decoder) noexcept;

        auto name() const noexcept -> const String &;

        auto encode(const Value &value) const noexcept -> std::pair<std::optional<String>, String>;

        auto decode(const std::string_view &content) const noexcept -> std::pair<std::optional<Value>, String>;

        static auto by_name(const String &name) noexcept -> const traeger_format_t *;

#if defined(TRAEGER_FORMAT_JSON)
        static auto json() noexcept -> const Format &;
#endif
#if defined(TRAEGER_FORMAT_YAML)
        static auto yaml() noexcept -> const Format &;
#endif
#if defined(TRAEGER_FORMAT_MSGPACK)
        static auto msgpack() noexcept -> const Format &;
#endif

    private:
        String name_;
        encoder_type *encoder_;
        decoder_type *decoder_;
    };
}

struct traeger_format_t : traeger::Format
{
};
