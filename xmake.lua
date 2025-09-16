-- xmake.lua
-- FEM (Finite Element Method) Project Configuration

-- set minimum required version
set_xmakever("2.3.1")

-- project
set_project("FEM")
set_version("1.0.0")

-- set language
set_languages("c++17")

-- add modes
add_rules("mode.debug", "mode.release")

-- target
target("fem")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")

    -- set warnings
    set_warnings("all", "error")

    -- debug
    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
    end

    -- release
    if is_mode("release") then
        set_symbols("hidden")
        set_optimize("fastest")
        set_strip("all")
    end