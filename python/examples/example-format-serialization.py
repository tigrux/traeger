# SPDX-License-Identifier: BSL-1.0

import traeger


def main():
    value: traeger.Value = traeger.Value(
        traeger.Map(
            {
                "name": "John",
                "age": 30,
                "married": True,
            }
        )
    )
    print(f"The original value is {value}")

    json: traeger.Format = traeger.Format("json")
    encoded = json.encode(value)
    print(f"The encoded json is {encoded}")

    decoded = json.decode(encoded)
    print(f"The decoded value is {decoded}")


if __name__ == "__main__":
    main()
