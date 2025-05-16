// SPDX-License-Identifier: BSL-1.0

use traeger::{unpack, Float};

pub mod example_actor_definition;
use example_actor_definition::make_account_actor;

traeger::module_init!(|map| {
    let initial_funds = match unpack!(map, "initial_funds" => Float) {
        Ok((value,)) => value,
        Err(_) => 0.0,
    };

    println!("initial_funds: {initial_funds}");
    let actor = make_account_actor(initial_funds);
    Ok(actor)
});

#[allow(dead_code)]
fn main() {}
