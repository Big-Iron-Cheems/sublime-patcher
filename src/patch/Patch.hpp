#pragma once

#include <vector>

/// A byte used to patch a binary file
struct PatchByte {
    unsigned char value; ///< The actual byte value
    bool isWildcard;     ///< Indicates if this is a wildcard

    PatchByte(const unsigned char v, const bool wildcard)
        : value(v),
          isWildcard(wildcard) {}
};

/// A patch is a pair of original and patched bytes used to modify a binary file
class Patch {
public:
    Patch(const std::string &original, const std::string &patched)
        : original(parseBytes(original)),
          patched(parseBytes(patched)) {
        if (this->original.size() != this->patched.size()) {
            throw std::invalid_argument(std::format("Original and patched bytes must have the same size (Original: {}, Patched: {})", this->original.size(), this->patched.size()));
        }
    }

    /// Find a pattern in a vector of bytes
    /// @param bytes a vector of bytes in which to search for the pattern
    /// @param pattern a vector of PatchByte to search for in the bytes
    /// @return the offset of the pattern in the bytes if found, otherwise std::nullopt
    static std::optional<size_t> findPatternInBytes(const std::vector<unsigned char> &bytes, const std::vector<PatchByte> &pattern) {
        if (pattern.empty() || bytes.empty() || pattern.size() > bytes.size()) {
            spdlog::warn("Invalid pattern or bytes: pattern size={}, bytes size={}", pattern.size(), bytes.size());
            return std::nullopt;
        }

        // Print pattern bytes for debugging
        std::string pattern_str;
        for (const auto &byte: pattern) {
            pattern_str += byte.isWildcard ? "?? " : std::format("{:02X} ", byte.value);
        }
        spdlog::info("Searching for pattern: {}", pattern_str);

        auto match_predicate = [](const unsigned char file_byte, const PatchByte &patch_byte) { return patch_byte.isWildcard || file_byte == patch_byte.value; };
        if (const auto result = std::ranges::search(bytes, pattern, match_predicate); !result.empty()) {
            size_t offset = std::distance(bytes.begin(), result.begin());

            // Print matched bytes for debugging
            std::string matched_str;
            for (auto it = result.begin(); it != result.begin() + pattern.size(); ++it) {
                matched_str += std::format("{:02X} ", *it);
            }
            spdlog::info("Found match at offset 0x{:X}: {}", offset, matched_str);

            return offset;
        }

        spdlog::info("Pattern not found");
        return std::nullopt;
    }

    const std::vector<PatchByte> original; ///< Original bytes
    const std::vector<PatchByte> patched;  ///< Patched bytes

private:
    /// Parse a string of bytes into a vector of PatchByte, where each byte is separated by a space and ?? is a wildcard
    static std::vector<PatchByte> parseBytes(const std::string &byteString) {
        std::vector<PatchByte> bytes;
        std::istringstream stream(byteString);
        std::string byte;

        while (stream >> byte) {
            if (byte == "??") bytes.emplace_back(static_cast<unsigned char>(0x00), true);
            else {
                const unsigned int value = std::stoul(byte, nullptr, 16);
                if (value > 0xFF) throw std::invalid_argument("Byte value must be between 0x00 and 0xFF");
                bytes.emplace_back(static_cast<unsigned char>(value), false);
            }
        }

        return bytes;
    }
};
