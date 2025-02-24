#include "app/SublimeApp.hpp"
#include "patch/PatchManager.hpp"

#include <filesystem>
#include <ranges>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

/// Try to find Sublime Text or Sublime Merge executable in the current directory
std::optional<std::filesystem::path> findExecutableInCurrentDir() {
    const std::array<std::string, 2> executable_names{"sublime_text", "sublime_merge"};

    for (const auto current_path = std::filesystem::current_path(); const auto &entry: std::filesystem::directory_iterator(current_path)) {
        const auto filename = entry.path().filename().string();

        for (const auto &name: executable_names) {
            if (filename == name || filename == name + ".exe") {
                spdlog::info("[+] Found {} in current directory: {}", filename.starts_with("sublime_text") ? "Sublime Text" : "Sublime Merge", entry.path().string());
                return entry.path();
            }
        }
    }

    return std::nullopt;
}

int patchFile(const std::filesystem::path &path, const PatchManager &patch_manager) {
    SublimeApp sublime_app(path);

    // Check if the Sublime Text/Merge version is already patched
    if (patch_manager.isAlreadyPatched(sublime_app)) return EXIT_SUCCESS;

    // Create a backup of the original file
    if (!sublime_app.createBackup()) return EXIT_FAILURE;

    // Apply patches
    if (!patch_manager.applyPatches(sublime_app)) return EXIT_FAILURE;

    // Save the patched file
    if (!sublime_app.savePatchedFile()) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int main(const int argc, char *argv[]) {
    // Init logger
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_pattern("[%^%l%$] %v");
    const auto program_name = std::filesystem::path(argv[0]).filename().string();

    try {
        if (argc > 2) {
            spdlog::error("Usage: {} [path_to_sublime_text.exe or sublime_merge.exe]", program_name);
            return EXIT_FAILURE;
        }

        std::optional<std::filesystem::path> file_path;
        if (argc == 2) file_path = argv[1];
        else {
            file_path = findExecutableInCurrentDir();
            if (!file_path) {
                spdlog::error("[-] Error: Could not find sublime_text.exe or sublime_merge.exe in current directory");
                spdlog::error("Usage: {} [path_to_sublime_text.exe or sublime_merge.exe]", program_name);
                return EXIT_FAILURE;
            }
        }

        PatchManager patch_manager;
        PatchManager::initPatches(patch_manager);
        return patchFile(*file_path, patch_manager);
    } catch (const std::exception &e) {
        spdlog::error("{}", e.what());
        return EXIT_FAILURE;
    }
}
