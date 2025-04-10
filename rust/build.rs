// \file Build script
// \copyright Randomcode Developers

use std::{
    env::var,
    io,
    path::{Path, PathBuf},
};

fn symlink<P, Q>(src: P, dest: Q) -> io::Result<()>
where
    P: AsRef<Path>,
    Q: AsRef<Path>,
{
    #[cfg(target_os = "windows")]
    return std::os::windows::fs::symlink_file(src, dest);

    #[cfg(target_os = "linux")]
    return std::os::unix::fs::symlink(src, dest);
}

fn main() {
    let out_path = PathBuf::from(var("OUT_DIR").unwrap());
    let profile = var("PROFILE").unwrap();
    let arch = var("CARGO_CFG_TARGET_ARCH")
        .unwrap()
        .replace("aarch64", "arm64")
        .replace("x86_64", "x64");
    let os = var("CARGO_CFG_TARGET_OS").unwrap();
    let lib_path = PathBuf::from(format!("../build/{os}/{arch}/{profile}"))
        .canonicalize()
        .unwrap();
    let lib_dir = lib_path.to_str().unwrap();
    println!("cargo::rustc-link-search=native={lib_dir}");

    if os == "windows" {
        println!("{out_path:#?}");
        let _ = symlink(lib_path.join("Base.dll"), out_path.join("../../../Base.dll"));
    }
}
