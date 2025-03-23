/// \file Bindings to Base_* functions
/// \copyright Randomcode Developers

use core::slice;
use std::{
    alloc::{self, AllocError, Layout},
    ffi::CString,
    ptr::{NonNull, null},
};

#[repr(C)]
pub struct DateTime {
    year: u32,
    month: u32,
    day: u32,

    hour: u32,
    minute: u32,
    second: u32,
    millisecond: u32,
    weekDay: u32,
}

impl DateTime {
    pub fn now(&mut self) {
        unsafe {
            super::Plat::GetDateTime(self);
        }
    }
}

#[link(name = "Base")]
unsafe extern "C" {
    #[cfg_attr(target_os = "windows", link_name = "?Base_Alloc@@YAPEAX_J0@Z")]
    #[cfg_attr(target_os = "linux", link_name = "_Z10Base_Allocll")]
    pub fn Alloc(size: usize, align: usize) -> *mut ();
    #[cfg_attr(target_os = "windows", link_name = "?Base_Realloc@@YAPEAXPEAX_J@Z")]
    #[cfg_attr(target_os = "linux", link_name = "_Z12Base_ReallocPvl")]
    pub fn Realloc(block: *const (), newSize: usize) -> *mut ();
    #[cfg_attr(target_os = "windows", link_name = "?Base_Free@@YAXPEAX@Z")]
    #[cfg_attr(target_os = "linux", link_name = "_Z9Base_FreePv")]
    pub fn Free(block: *const ());
    #[cfg_attr(target_os = "windows", link_name = "?Base_AbortSafe@@YAXHPEBD@Z")]
    #[cfg_attr(target_os = "linux", link_name = "_Z14Base_AbortSafeiPKc")]
    pub fn AbortSafe(code: i32, msg: *const i8) -> !;
}

// Better to just reimplement this, rather than wrap it
pub fn Quit(msg: &str) -> ! {
    FatalError!("{msg}");
    let rawMsg = CString::new(msg).unwrap();
    unsafe {
        AbortSafe(i32::MAX, rawMsg.as_ptr());
    }
}

pub struct Allocator;

unsafe impl alloc::GlobalAlloc for Allocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        unsafe { Alloc(layout.size(), layout.align()) as *mut u8 }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
        unsafe { Free(ptr as *const ()) }
    }
}
