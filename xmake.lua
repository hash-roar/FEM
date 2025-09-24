set_project("MyAwesomeProject")
set_version("1.0.0")
set_description("FEM using c++20")
add_rules("mode.debug", "mode.release")
set_defaultmode("debug")
add_requires("fmt",{system = false})
add_requires("eigen",{system = false})
add_requires("gtest",{system = false})


option("enable_feature")
    set_default(false) 
    set_showmenu(true) 
    set_description("Enable a custom feature")

option("enable_tests")
    set_default(true) 
    set_showmenu(true) 
    set_description("Enable unit tests")


target("fem")
    set_languages("c++20")
    set_kind("binary")        
    add_files("src/*.cpp", "src/**.cpp")
    add_includedirs("src")
    -- includes("src")
    add_packages("fmt")
    add_packages("eigen")    

    if is_mode("release") then

        set_optimize("fastest")

        set_strip("all")
        add_defines("NDEBUG")

    end    
    if is_mode("debug") then

        set_optimize("none")

        set_symbols("debug")

        add_defines("_DEBUG", "DEBUG")

    end


    if is_plat("windows") then
        
        add_defines("PLATFORM_WINDOWS")
        
        add_cxflags("/W4")
    elseif is_plat("linux") then
        
        add_defines("PLATFORM_LINUX")
        add_cxflags("-Wall", "-Wextra", "-Wpedantic")
    elseif is_plat("macosx") then
        
        add_defines("PLATFORM_MACOS")
        add_cxflags("-Wall", "-Wextra", "-Wpedantic")
    end    

-- Add test target
if has_config("enable_tests") then
    target("fem_tests")
        set_default(false)
        set_kind("binary")
        set_languages("c++20")
        add_files("tests/unit/*.cpp", "tests/integration/*.cpp", "tests/utils/*.cpp", "tests/test_main.cpp")
        add_files("src/fem_mesh_loader.cpp", "src/mesh/triangle_element.cpp") -- Include source files needed for testing
        add_includedirs("src", "tests")
        add_packages("gtest", "fmt", "eigen")
        add_defines("TESTING")
        
        if is_mode("debug") then
            set_optimize("none")
            set_symbols("debug")
            add_defines("_DEBUG", "DEBUG")
        end
        
        after_build(function (target)
            print("Tests built successfully. Run './build/linux/x86_64/debug/fem_tests' to execute tests.")
        end)
end
    if get_config("enable_feature") then
        
        add_defines("ENABLE_MY_FEATURE")
        
        
    end