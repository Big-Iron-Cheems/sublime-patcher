#include "app/SublimeApp.hpp"
#include "patch/PatchManager.hpp"

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

/// Program options set via CLI
struct ProgramOptions {
    std::string binary_path;
    bool scan_fs = false;
    std::string log_level = "info";
    bool backup = true;
};

/// Parse CLI options via CLI11
ProgramOptions parseCli(const int argc, char *argv[]) {
    ProgramOptions options;
    CLI::App app{"Sublime Text/Merge License Patcher"};
    app.name(std::filesystem::path(argv[0]).filename().string());
    app.get_formatter()->column_width(40);

    app.add_option("sublime_binary", options.binary_path, "Path to Sublime Text/Merge binary")->check(CLI::ExistingFile)->type_size(0);
    app.add_flag("--scan-fs,-s", options.scan_fs, "Scan filesystem for Sublime binaries");
    app.add_option("--log-level,-l", options.log_level, "Log level (trace, debug, info, warn, error, critical)")
            ->check(CLI::IsMember({"trace", "debug", "info", "warn", "error", "critical"}))
            ->type_size(0);
    app.add_flag("!--no-backup,!-b", options.backup, "Disable backup creation");

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &) {
        std::cout << app.help() << std::endl;
        std::exit(EXIT_SUCCESS);
    } catch (const CLI::ParseError &e) {
        app.exit(e);
        throw;
    }

    return options;
}

/// Initialize spdlog logger
void initLogger(const ProgramOptions &options) {
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_pattern("[%^%l%$] %v");

    spdlog::level::level_enum log_level_enum = spdlog::level::info;
    if (options.log_level == "trace") log_level_enum = spdlog::level::trace;
    else if (options.log_level == "debug") log_level_enum = spdlog::level::debug;
    else if (options.log_level == "warn") log_level_enum = spdlog::level::warn;
    else if (options.log_level == "error") log_level_enum = spdlog::level::err;
    else if (options.log_level == "critical") log_level_enum = spdlog::level::critical;
    spdlog::set_level(log_level_enum);

    if (!options.backup) spdlog::warn("Backup creation is disabled, you may corrupt your original file");
}

/// Try to find Sublime Text or Sublime Merge executable in the current directory or default paths
std::optional<std::filesystem::path> findExecutable(const ProgramOptions &options) {
    std::vector<std::filesystem::path> found_executables;
    const auto executable_names = std::to_array<std::string>({"sublime_text", "sublime_merge"});
    std::vector<std::filesystem::path> search_paths;

    if (options.scan_fs) {
        search_paths = {
                std::filesystem::current_path(),
#if defined(_WIN32)
                std::filesystem::path("C:/Program Files/Sublime Text"),
                std::filesystem::path("C:/Program Files (x86)/Sublime Text"),
                std::filesystem::path("C:/Program Files/Sublime Merge"),
                std::filesystem::path("C:/Program Files (x86)/Sublime Merge"),
#elif defined(__APPLE__)
                std::filesystem::path("/Applications/Sublime Text.app/Contents/SharedSupport/bin"),
                std::filesystem::path("/Applications/Sublime Merge.app/Contents/SharedSupport/bin"),
#else
                std::filesystem::path("/usr/bin"),
                std::filesystem::path("/usr/local/bin"),
#endif
        };
    } else search_paths = {std::filesystem::current_path()};

    for (const auto &current_path: search_paths) {
        for (const auto &entry: std::filesystem::directory_iterator(current_path)) {
            const auto filename = entry.path().filename().string();

            for (const auto &name: executable_names) {
                if (filename == name || filename == name + ".exe") {
                    found_executables.push_back(entry.path());
                    spdlog::info("[+] Found {} in directory: {}", filename.starts_with("sublime_text") ? "Sublime Text" : "Sublime Merge", entry.path().string());
                }
            }
        }
    }

    if (found_executables.empty()) return std::nullopt;
    if (found_executables.size() == 1) return found_executables[0];

    // Prompt user when single executable can't be determined
    std::stringstream menu;
    menu << "Multiple executables found, please specify which one to patch:\n";
    for (size_t i = 0; i < found_executables.size(); ++i) {
        menu << std::format("[{}] {}\t({})\n", i + 1, found_executables[i].filename().string(), found_executables[i].string());
    }
    spdlog::info(menu.str());

    size_t choice;
    while (true) {
        std::cout << "Enter number (1-" << found_executables.size() << "): ";
        if (std::cin >> choice && choice >= 1 && choice <= found_executables.size()) {
            break;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        spdlog::error("Invalid input, please enter a number between 1 and {}", found_executables.size());
    }

    return found_executables[choice - 1];
}

int patchFile(const std::filesystem::path &path, const PatchManager &patch_manager, const ProgramOptions &options) {
    auto sublime_app = SublimeApp::create(path);

    // Check if the Sublime Text/Merge version is already patched
    if (patch_manager.isAlreadyPatched(sublime_app)) return EXIT_SUCCESS;

    // Create a backup of the original file
    if (options.backup && !sublime_app.createBackup()) return EXIT_FAILURE;

    // Apply patches
    if (!patch_manager.applyPatches(sublime_app)) return EXIT_FAILURE;

    // Save the patched file
    if (!sublime_app.savePatchedFile()) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int main(const int argc, char *argv[]) {
    try {
        const auto options = parseCli(argc, argv);
        initLogger(options);

        std::optional<std::filesystem::path> file_path;
        if (!options.binary_path.empty()) file_path = options.binary_path;
        else {
            file_path = findExecutable(options);
            if (!file_path) {
                spdlog::error("[-] Error: Could not find Sublime application binary");
                return EXIT_FAILURE;
            }
        }

        PatchManager patch_manager;
        PatchManager::initPatches(patch_manager);
        return patchFile(*file_path, patch_manager, options);
    } catch (const std::exception &e) {
        spdlog::error("{}", e.what());
        return EXIT_FAILURE;
    }
}
