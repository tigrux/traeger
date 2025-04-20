// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <traeger/value/Value.hpp>

int main()
{
    auto list1 = traeger::make_list(2, 3, 5);
    std::cout << "This is list1: " << list1 << std::endl;

    auto list2 = list1;
    list2.set(0, 1);
    list2.append(7);
    std::cout << "The list2 has odd numbers: " << list2 << std::endl;

    auto list3 = list1;
    list3.append(7);
    std::cout << "The list3 has prime numbers: " << list3 << std::endl;

    auto map1 = traeger::make_map(
        "odds", list2,
        "primes", list3);
    std::cout << "A map with the numbers:" << std::endl;
    for (const auto [key, value] : map1)
    {
        std::cout << key << ": " << value << std::endl;
    }
}
