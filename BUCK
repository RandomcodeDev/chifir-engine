# A list of available rules and their signatures can be found here: https://buck2.build/docs/api/rules/

load("@prelude//toolchains:cxx.bzl", "system_cxx_toolchain")

system_cxx_toolchain(
    name = "cxx",
    visibility = ["PUBLIC"],
)

cxx_binary(
    name = "launcher",
    srcs = ["launcher/entry.cpp"],
    link_style = "static",
)
