// \file Library test
// \copyright 2025 Randomcode Developers

#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(unused)]

use chifir::Info;

unsafe extern "C" fn CreateInterface() {
    Info!("rust dll");
}
