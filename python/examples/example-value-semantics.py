# SPDX-License-Identifier: BSL-1.0

import traeger


def main():
    list1: traeger.List = traeger.List([2, 3, 5])
    print(f"This is list1: {list1}")

    list2: traeger.List = list1.copy()
    list2[0] = 1
    list2.append(7)
    print(f"The list2 has odd numbers: {list2}")

    list3: traeger.List = list1.copy()
    list3.append(7)
    print(f"The list2 has prime numbers: {list3}")

    map1: traeger.Map = traeger.Map(
        {
            "odds": list2,
            "primes": list3,
        }
    )
    print(f"A map with the numbers:")
    for key, value in map1.items():
        print(f"{key}: {value}")


if __name__ == "__main__":
    main()
