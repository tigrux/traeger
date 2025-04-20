// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <traeger/value/Value.hpp>
#include <traeger/format/Format.hpp>

int main()
{
    auto value = traeger::Value{
        traeger::make_map(
            "name", "John",
            "age", 30,
            "married", true)};
    std::cout << "The original value is " << value << std::endl;

    const auto &json = traeger::Format::json();
    auto [encode_optional, encode_error] = json.encode(value);
    if (!encode_optional)
    {
        std::cerr << "Encode error " << encode_error << std::endl;
        return 1;
    }

    const auto encoded_string = encode_optional.value();
    std::cout << "The encoded json is " << encoded_string << std::endl;

    auto [decode_optional, decode_error] = json.decode(encoded_string);
    if (!decode_optional)
    {
        std::cerr << "Decode error " << decode_error << std::endl;
        return 1;
    }

    const auto decoded_value = decode_optional.value();
    std::cout << "The decoded value is " << decoded_value << std::endl;
}
