// \file Bindings to Log_* functions
// \copyright Randomcode Developers

use std::{ffi::CString, ptr};

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
    #[cfg_attr(
        target_os = "windows",
        link_name = "?Log_Write@@YAXW4LogLevel_t@@_K_NPEBD33ZZ"
    )]
    #[cfg_attr(
        target_os = "linux",
        link_name = "_Z9Log_Write10LogLevel_tmbPKcS1_S1_z"
    )]
    pub fn Write(
        level: Level,
        location: u64,
        isAddress: bool,
        file: *const i8,
        function: *const i8,
        threadName: *const i8,
        threadId: u64,
        message: *const i8,
    );
}

pub fn WriteSafe(
    level: Level,
    location: u32,
    is_addr: bool,
    file: &str,
    function: &str,
    thread_name: Option<&str>,
    thread_id: u64,
    message: &str,
) {
    unsafe {
        let thread_name_ptr = if let Some(thread_name) = thread_name {
            CString::new(thread_name).unwrap().as_ptr()
        } else {
            ptr::null()
        };

        Write(
            level,
            location.into(),
            is_addr,
            CString::new(file).unwrap().as_ptr(),
            CString::new(function).unwrap().as_ptr(),
            thread_name_ptr,
            thread_id,
            CString::new(message).unwrap().as_ptr(),
        )
    }
}
