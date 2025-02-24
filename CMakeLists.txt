cmake_minimum_required(VERSION 3.30)
project(sublime_patcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # Enforce the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler-specific extensions

include(FetchContent)

set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format in spdlog")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.1
)
FetchContent_MakeAvailable(spdlog)

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

target_link_libraries(sublime_patcher PRIVATE spdlog::spdlog)

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
