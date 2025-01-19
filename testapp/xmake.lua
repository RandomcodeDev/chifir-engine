target("TestApp")
    add_defines("IN_TESTAPP")

    add_headerfiles("**.h")
    add_files("interface.cpp", "testapp.cpp")

    add_deps("Base", "CommonFiles", "DllSupport")
target_end()
