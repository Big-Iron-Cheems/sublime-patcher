#pragma once

#include "Architecture.hpp"
#include "Platform.hpp"

struct AppInfo {
    std::string build_version; ///< Build version of the Sublime Text/Merge executable
    std::string major_version; ///< Major version of the Sublime Text/Merge executable
    Platform platform;         ///< Platform of the Sublime Text/Merge executable
    Architecture arch;         ///< Architecture of the Sublime Text/Merge executable
};
