-- xmake.lua

-- --------------------------------------------------------------------------
-- 项目元数据
-- --------------------------------------------------------------------------
-- 设置项目名称、版本和描述
set_project("MyAwesomeProject")
set_version("1.0.0")
set_description("A detailed boilerplate for a C++20 project using xmake.")

-- --------------------------------------------------------------------------
-- 全局构建规则和设置
-- --------------------------------------------------------------------------
-- 添加默认的构建模式：debug 和 release
-- 这允许我们使用 `xmake f -m debug` 或 `xmake f -m release` 来切换模式
add_rules("mode.debug", "mode.release")

-- 设置默认的构建模式为 debug
-- 如果不通过 `xmake f -m ...` 指定，则默认使用此模式
set_defaultmode("debug")

add_requires("fmt",{system = false})
add_requires("eigen",{system = false})
-- --------------------------------------------------------------------------
-- 自定义选项 (Optional)
-- --------------------------------------------------------------------------
-- 添加一个自定义的编译选项，可以在配置时开启或关闭
-- 使用 `xmake f --enable_feature=true` 来启用它
option("enable_feature")
    set_default(false) -- 默认关闭
    set_showmenu(true) -- 在 `xmake f --menu` 中显示
    set_description("Enable a custom feature")

-- --------------------------------------------------------------------------
-- 依赖包管理 (Package Management)
-- --------------------------------------------------------------------------
-- 声明项目需要的远程依赖包
-- xmake 会自动从 xmake-repo 或其他配置的仓库下载和构建它们
-- 示例：添加 {fmt} 库作为依赖


-- --------------------------------------------------------------------------
-- 定义目标 (Target)
-- --------------------------------------------------------------------------
-- 定义一个名为 "MyApp" 的可执行文件目标
target("fem")
    -- set_toolchains("clang-20")
    set_languages("c++20")
    -- 设置目标的类型为可执行文件 ("binary")
    -- 其他可选值: "static", "shared", "headeronly"
    set_kind("binary")

    -- 关键：设置项目的 C++ 标准为 C++20

    -- 添加源文件
    -- "src/*.cpp": 添加 src 目录下所有 .cpp 文件
    -- "src/**.cpp": 递归添加 src 目录下所有子目录的 .cpp 文件
    add_files("src/*.cpp", "src/**.cpp")

    -- 将声明的依赖包链接到当前目标
    add_packages("fmt")
    add_packages("eigen")

    -- 添加包含目录
    -- add_includedirs("includes")

    -- 添加链接库
    -- add_links("mylib")

    -- ----------------------------------------------------------------------
    -- 特定于构建模式的配置
    -- ----------------------------------------------------------------------
    -- 如果是 release 模式
    if is_mode("release") then
        -- 开启优化
        set_optimize("fastest")
        -- 移除符号信息，减小最终文件大小
        set_strip("all")
        -- 添加宏定义，例如 NDEBUG 来禁用断言
        add_defines("NDEBUG")
        -- 你也可以在这里添加特定于 release 的其他标志
        -- add_cxflags("-O3")
    end

    -- 如果是 debug 模式
    if is_mode("debug") then
        -- 关闭优化，便于调试
        set_optimize("none")
        -- 开启并嵌入调试符号
        set_symbols("debug")
        -- 添加宏定义，例如 _DEBUG
        add_defines("_DEBUG", "DEBUG")
        -- 你也可以在这里添加特定于 debug 的其他标志
        -- add_cxflags("-g")
    end

    -- ----------------------------------------------------------------------
    -- 特定于平台的配置
    -- ----------------------------------------------------------------------
    if is_plat("windows") then
        -- Windows 平台特定配置
        add_defines("PLATFORM_WINDOWS")
        -- 在 MSVC 上开启更严格的警告等级
        add_cxflags("/W4")
    elseif is_plat("linux") then
        -- Linux 平台特定配置
        add_defines("PLATFORM_LINUX")
        add_cxflags("-Wall", "-Wextra", "-Wpedantic")
    elseif is_plat("macosx") then
        -- macOS 平台特定配置
        add_defines("PLATFORM_MACOS")
        add_cxflags("-Wall", "-Wextra", "-Wpedantic")
    end

    -- ----------------------------------------------------------------------
    -- 基于自定义选项的条件编译
    -- ----------------------------------------------------------------------
    if get_config("enable_feature") then
        -- 如果用户启用了 "enable_feature" 选项
        add_defines("ENABLE_MY_FEATURE")
        -- 也可以添加特定的源文件或库
        -- add_files("src/feature.cpp")
    end

-- --------------------------------------------------------------------------
-- 自定义任务 (Optional)
-- --------------------------------------------------------------------------
-- 定义一个可以通过 `xmake hello` 运行的自定义任务
task("hello")
    on_run(function ()
        print("Hello from xmake custom task!")
        print("Project: %s", get_project())
        print("Version: %s", get_version())
    end)
    set_menu {
        -- 设置在 `xmake -h` 中的描述信息
        category = "Other",
        description = "Prints a hello message"
    }
