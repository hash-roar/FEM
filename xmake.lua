set_project("MyAwesomeProject")
set_version("1.0.0")
set_description("FEM using c++20")
add_rules("mode.debug", "mode.release")
set_defaultmode("debug")
add_requires("fmt",{system = false})
add_requires("eigen",{system = false})


option("enable_feature")
    set_default(false) 
    set_showmenu(true) 
    set_description("Enable a custom feature")


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
    
    
    if get_config("enable_feature") then
        
        add_defines("ENABLE_MY_FEATURE")
        
        
    end