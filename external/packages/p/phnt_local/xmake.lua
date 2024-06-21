package("phnt_local")
    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/winsiderss/systeminformer")
    set_description("Native API header files for the System Informer project.")

    set_urls("https://github.com/winsiderss/systeminformer.git")
    add_versions("", "5318eceb8ca727dba8b08be50ae799a42978107a")

    on_install("gdk", "gdkx", "xbox360", "windows", function (package)
        os.cp("phnt/include/*.h", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:has_cincludes("phnt_windows.h"))
    end)
package_end()
