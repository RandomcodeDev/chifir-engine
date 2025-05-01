for %%x in (arm64 x64 x86) do (
    link /lib /def:%~dp0ntdll_libc_%%x.def /machine:%%x /out:%~dp0%%x\ntdll_libc.lib
    del %~dp0%%x\ntdll_libc.exp
)
