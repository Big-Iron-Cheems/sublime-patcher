#pragma once

#include "../core/AppInfo.hpp"
#include "../core/AppType.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <spdlog/spdlog.h>

class SublimeApp {
public:
    explicit SublimeApp(std::filesystem::path file_path);
    friend std::ostream &operator<<(std::ostream &os, const SublimeApp &app);

    /// Create a backup of the original file
    bool createBackup() const;
    /// Save the patched file to the original path
    bool savePatchedFile() const;

    const std::filesystem::path file_path;      ///< Path to the Sublime Text/Merge executable
    const std::vector<unsigned char> file_data; ///< Raw file bytes of the executable
    std::vector<unsigned char> patched_data;    ///< Patched file bytes of the patched executable
    const AppInfo app_info;                     ///< Information extracted from the executable
    const AppType app_type;                     ///< Type of the Sublime application

private:
    /// Load the file data into memory
    std::vector<unsigned char> loadFileData() const;
    /// Extract version, platform, and architecture information from the executable
    AppInfo extractFileInfo(const std::vector<unsigned char> &data) const;
    /// Detect the type of the Sublime application
    static AppType detectAppType(const std::filesystem::path &file_path);
};

template<>
struct std::formatter<SublimeApp> : std::formatter<std::string_view> {
    auto format(const SublimeApp &app, std::format_context &ctx) const -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "{} {} (Version: {}, OS: {}, Arch: {})", app.app_type, app.app_info.major_version, app.app_info.build_version, app.app_info.platform,
                              app.app_info.arch);
    }
};
