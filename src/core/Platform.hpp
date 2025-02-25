#pragma once

#include <format>
#include <stdexcept>

enum class Platform { Unknown, Windows, macOS, Linux };

namespace platform {
constexpr std::string_view to_string(const Platform platform) {
    switch (platform) {
        using enum Platform;
        case Unknown: return "Unknown";
        case Windows: return "Windows";
        case macOS: return "macOS";
        case Linux: return "Linux";
    }
    throw std::invalid_argument("Invalid platform value");
}

constexpr Platform from_string(const std::string_view str) {
    using enum Platform;
    if (str == "windows") return Windows;
    if (str == "osx") return macOS;
    if (str == "linux") return Linux;
    return Unknown;
}
} // namespace platform

inline std::ostream &operator<<(std::ostream &os, const Platform platform) { return os << platform::to_string(platform); }

template<>
struct std::formatter<Platform> : std::formatter<std::string_view> {
    auto format(const Platform platform, format_context &ctx) const -> decltype(ctx.out()) { return std::formatter<std::string_view>::format(platform::to_string(platform), ctx); }
};
