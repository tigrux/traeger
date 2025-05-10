// SPDX-License-Identifier: BSL-1.0

use std::str::from_utf8_unchecked;

use traeger::{Format, Variant, map};

fn main() {
    let variant = Variant::Map(map!(
        "name" => "John",
        "age" => 30,
        "married" => true
    ));
    println!("The original variant is {variant:?}");

    let json = Format::by_name("json").unwrap();
    match json.encode(variant) {
        Err(error) => {
            println!("Encode error {error}");
        }
        Ok(data) => {
            println!("The encoded json is {}", unsafe {
                from_utf8_unchecked(&data)
            });
            match json.decode(&data) {
                Err(error) => {
                    println!("Decode error {error}");
                }
                Ok(variant) => {
                    println!("The decoded variant is {variant:?}");
                }
            }
        }
    }
}
