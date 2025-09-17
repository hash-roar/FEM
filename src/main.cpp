// src/main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <span>

// #include <format>
// 包含由 xmake 自动管理的 fmt 库
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <Eigen/Dense>

void print_build_info() {
    fmt::print("--- Build Information ---\n");
#if defined(NDEBUG)
    fmt::print("Build Mode: Release\n");
#elif defined(_DEBUG)
    fmt::print("Build Mode: Debug\n");
#else
    fmt::print("Build Mode: Unknown\n");
#endif

#if defined(PLATFORM_WINDOWS)
    fmt::print("Platform: Windows\n");
#elif defined(PLATFORM_LINUX)
    fmt::print("Platform: Linux\n");
#elif defined(PLATFORM_MACOS)
    fmt::print("Platform: macOS\n");
#else
    fmt::print("Platform: Unknown\n");
#endif

#if defined(ENABLE_MY_FEATURE)
    fmt::print("Feature 'My Feature' is ENABLED.\n");
#else
    fmt::print("Feature 'My Feature' is DISABLED.\n");
#endif
    fmt::print("-------------------------\n\n");
}

int main() {
    print_build_info();

    fmt::print("Hello, C++20 and xmake!\n");

    std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
    fmt::print("Names: {}\n", names);
    // C++20 ranges
    for (int i : std::views::iota(1, 5)) {
        fmt::print("Number: {}\n", i);
    }
    // test std::span
    Eigen::MatrixXd mat(2, 2);
    mat(0, 0) = 1;
    mat(0, 1) = 2;
    // print matrix using fmt
    for(int i = 0; i < mat.rows(); i++) {
        for(int j = 0; j < mat.cols(); j++) {
            fmt::print("{} ", mat(i, j));
        }
        fmt::print("\n");
    }
    int arr[] = {10, 20, 30, 40, 50};
    std::span span_arr(arr);
    fmt::print("Span elements: {}\n", span_arr);

    return 0;
}
