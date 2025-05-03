// \file Bindings to Plat_* functions
// \copyright 2025 Randomcode Developers

#[link(name = "Base")]
unsafe extern "C" {
    #[cfg_attr(target_os = "linux", link_name = "_Z25Plat_GetSystemDescriptionv")]
    pub fn GetSystemDescription() -> *const i8;

    #[cfg_attr(target_os = "linux", link_name = "_Z27Plat_GetHardwareDescriptionv")]
    pub fn GetHardwareDescription() -> *const i8;

    #[cfg_attr(target_os = "linux", link_name = "_Z17Plat_WriteConsolePKc")]
    pub fn WriteConsole(message: *const i8);

    pub fn ConsoleHasColor() -> bool;

    pub fn GetMilliseconds() -> u64;

    pub fn Sleep(milliseconds: u64);

    #[cfg_attr(target_os = "windows", link_name = "?Plat_GetDateTime@@YAXAEAUDateTime@@_N@Z")]
    #[cfg_attr(target_os = "linux", link_name = "_Z16Plat_GetDateTimeR8DateTimeb")]
    pub fn GetDateTime(time: &mut crate::sys::Base::DateTime);

    pub fn GetSaveLocation() -> *const i8;

    pub fn GetEngineDir() -> *const i8;
}
