#pragma once

#include <format>
#include <stdexcept>

enum class Architecture { Unknown, x86, x64, ARM64 };

namespace architecture {
constexpr std::string_view to_string(const Architecture arch) {
    switch (arch) {
        using enum Architecture;
        case Unknown: return "Unknown";
        case x86: return "x86";
        case x64: return "x64";
        case ARM64: return "ARM64";
    }
    throw std::invalid_argument("Invalid architecture value");
}

constexpr Architecture from_string(const std::string_view str) {
    using enum Architecture;
    if (str == "x86") return x86;
    if (str == "x64") return x64;
    if (str == "ARM64") return ARM64;
    return Unknown;
}
} // namespace architecture

inline std::ostream &operator<<(std::ostream &os, const Architecture arch) { return os << architecture::to_string(arch); }

template<>
struct std::formatter<Architecture> : std::formatter<std::string_view> {
    auto format(const Architecture arch, format_context &ctx) const -> decltype(ctx.out()) { return std::formatter<std::string_view>::format(architecture::to_string(arch), ctx); }
};
