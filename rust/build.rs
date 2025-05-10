use std::env;

fn main() {
    let home = env::var("HOME").unwrap_or("/usr/local".into());
    println!("cargo:rustc-link-search={home}/local/lib");
}
