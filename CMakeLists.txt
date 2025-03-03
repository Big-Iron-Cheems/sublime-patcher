cmake_minimum_required(VERSION 3.14)
project(sublime_patcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # Enforce the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler-specific extensions

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for macOS")
endif ()

include(FetchContent)

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.5/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=c46b876ae3b9f994b4f05a4c15553e0485636862064f1fcc9d8b4f832086bc5d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format in spdlog")
CPMAddPackage("gh:gabime/spdlog@1.15.1")
CPMAddPackage("gh:CLIUtils/CLI11@2.5.0")

add_executable(sublime_patcher
        src/main.cpp
        src/app/SublimeApp.cpp
        src/app/SublimeApp.hpp
        src/core/Architecture.hpp
        src/core/AppInfo.hpp
        src/core/AppType.hpp
        src/core/Platform.hpp
        src/patch/Patch.hpp
        src/patch/PatchManager.cpp
        src/patch/PatchManager.hpp
)

target_link_libraries(sublime_patcher PRIVATE spdlog::spdlog CLI11::CLI11)

if (MSVC)
    target_compile_options(sublime_patcher PRIVATE /W4 /permissive- /utf-8)
    target_compile_options(sublime_patcher PRIVATE "$<$<CONFIG:Debug>:/Od>")
    target_compile_options(sublime_patcher PRIVATE "$<$<CONFIG:Release>:/O2>")
    target_link_options(sublime_patcher PRIVATE "$<$<CONFIG:Release>:/LTCG>")
else ()
    target_compile_options(sublime_patcher PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(sublime_patcher PRIVATE "$<$<CONFIG:Debug>:-O0>")
    target_compile_options(sublime_patcher PRIVATE "$<$<CONFIG:Release>:-O3>")
    target_link_options(sublime_patcher PRIVATE "$<$<CONFIG:Release>:-flto>")
endif ()
