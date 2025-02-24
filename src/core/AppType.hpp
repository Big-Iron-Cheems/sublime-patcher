#pragma once

#include <format>
#include <stdexcept>

enum class AppType { Unknown, SublimeText, SublimeMerge };

namespace appType {
constexpr std::string_view to_string(const AppType type) {
    switch (type) {
        using enum AppType;
        case Unknown: return "Unknown";
        case SublimeText: return "Sublime Text";
        case SublimeMerge: return "Sublime Merge";
    }
    throw std::invalid_argument("Invalid application type value");
}

constexpr AppType from_string(const std::string_view str) {
    using enum AppType;
    if (str == "Sublime Text") return SublimeText;
    if (str == "Sublime Merge") return SublimeMerge;
    return Unknown;
}
} // namespace appType

inline std::ostream &operator<<(std::ostream &os, const AppType type) { return os << appType::to_string(type); }

template<>
struct std::formatter<AppType> : std::formatter<std::string_view> {
    auto format(const AppType type, format_context &ctx) const -> decltype(ctx.out()) { return std::formatter<std::string_view>::format(appType::to_string(type), ctx); }
};
