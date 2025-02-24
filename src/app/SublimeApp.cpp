#include "SublimeApp.hpp"

SublimeApp::SublimeApp(std::filesystem::path file_path)
    : file_path(std::move(file_path)),
      file_data(loadFileData()),
      patched_data(file_data),
      app_info(extractFileInfo(file_data)),
      app_type(detectAppType(this->file_path)) {
    spdlog::info("[+] Detected {}", *this);
}

std::ostream &operator<<(std::ostream &os, const SublimeApp &app) {
    return os << std::format("{} {} (Version: {}, OS: {}, Arch: {})", app.app_type, app.app_info.major_version, app.app_info.build_version, app.app_info.platform,
                             app.app_info.arch);
}

bool SublimeApp::createBackup() const {
    try {
        const auto backup_path = file_path.parent_path() / (file_path.filename().string() + ".bak");
        if (exists(backup_path)) spdlog::warn("[!] Warning: Backup file already exists: {}. Overwriting it.", backup_path.string());
        copy(file_path, backup_path, std::filesystem::copy_options::overwrite_existing);
        spdlog::info("[+] Backup created: {}", backup_path.string());
        return true;
    } catch (const std::filesystem::filesystem_error &e) {
        spdlog::error("[-] Error: Could not create backup: {}", e.what());
        return false;
    }
}

bool SublimeApp::savePatchedFile() const {
    try {
        std::ofstream patched_file(file_path, std::ios::binary);
        if (!patched_file) {
            spdlog::error("[-] Error: Could not open the patched file for writing");
            return false;
        }
        patched_file.write(reinterpret_cast<const char *>(patched_data.data()), patched_data.size());
        spdlog::info("[+] Patched file saved: {}", file_path.string());
        return true;
    } catch (const std::ofstream::failure &e) {
        spdlog::error("[-] Error: Could not save the patched file: {}", e.what());
        return false;
    }
}

std::vector<unsigned char> SublimeApp::loadFileData() const {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[-] Error: Could not open file: " + file_path.string());
    }

    std::vector<unsigned char> data((std::istreambuf_iterator(file)), {});
    if (data.empty()) {
        throw std::runtime_error("[-] Error: File is empty or unreadable: " + file_path.string());
    }

    return data;
}

// Private methods

AppType SublimeApp::detectAppType(const std::filesystem::path &file_path) {
    const std::string filename = file_path.filename().string();
    if (filename.contains("sublime_text")) return AppType::SublimeText;
    if (filename.contains("sublime_merge")) return AppType::SublimeMerge;
    throw std::runtime_error("[-] Error: Unknown Sublime application: " + filename);
}

AppInfo SublimeApp::extractFileInfo(const std::vector<unsigned char> &data) const {
    const std::string content(data.begin(), data.end());
    static const std::regex regex_pattern(R"(version=(\d+)&platform=(\w+)&arch=(\w+))");

    std::smatch match;
    if (!std::regex_search(content, match, regex_pattern)) {
        throw std::runtime_error("[-] Error: Could not find version, platform, and architecture in file: " + file_path.string());
    }

    const std::string build_version = match[1].str();
    const std::string major_version = build_version.substr(0, 1);
    const Platform platform = platform::from_string(match[2].str());
    const Architecture arch = architecture::from_string(match[3].str());

    return {build_version, major_version, platform, arch};
}
