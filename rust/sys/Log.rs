/// Bindings to Log_* functions
/// Copyright 2025 Randomcode Developers

use std::ffi::{CStr, CString};

#[repr(u32)]
pub enum Level {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    FatalError,
}

#[link(name = "Base")]
unsafe extern "C" {
    #[cfg_attr(target_os = "windows", link_name = "?Log_Write@@YAXW4LogLevel_t@@_K_NPEBD33ZZ")]
    #[cfg_attr(target_os = "linux", link_name = "_Z9Log_Write10LogLevel_tmbPKcS1_S1_z")]
    pub fn Write(
        level: Level,
        location: u64,
        isAddress: bool,
        file: *const i8,
        function: *const i8,
        message: *const i8,
    );
}

pub fn WriteSafe(
    level: Level,
    location: u32,
    is_addr: bool,
    file: &str,
    function: &str,
    message: &str,
) {
    unsafe {
        Write(
            level,
            location.into(),
            is_addr,
            CString::new(file).unwrap().as_ptr(),
            CString::new(function).unwrap().as_ptr(),
            CString::new(message).unwrap().as_ptr(),
        )
    }
}
