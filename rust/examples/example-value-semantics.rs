// SPDX-License-Identifier: BSL-1.0

use traeger::{Append, SetIndex, list, map};

fn main() {
    let list1 = list!(2, 3, 5);
    println!("This is list1: {list1}");

    let mut list2 = list1.clone();
    list2.set(0, 1);
    list2.append(7);
    println!("The listt2 has odd numbers: {list2}");

    let mut list3 = list1.clone();
    list3.append(7);
    println!("The list3 has prime numbers: {list3}");

    let map = map!(
        "odds" => &list2,
        "primes" => &list3
    );
    println!("A map with the numbers:");
    for (key, value) in &map {
        println!("{key}: {value}");
    }
}
