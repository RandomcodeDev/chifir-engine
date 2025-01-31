#[link(name = "Base")]
unsafe extern "C" {
    #[cfg_attr(target_os = "windows", link_name = "?Plat_GetDateTime@@YAXAEAUDateTime@@_N@Z")]
    #[cfg_attr(target_os = "linux", link_name = "_Z16Plat_GetDateTimeR8DateTimeb")]
    pub fn GetDateTime(time: &mut crate::Base::DateTime);
}
