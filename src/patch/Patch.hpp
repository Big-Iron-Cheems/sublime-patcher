#pragma once

#include <vector>

/// A patch is a pair of original and patched bytes used to modify a binary file
class Patch {
public:
    Patch(std::vector<unsigned char> original, std::vector<unsigned char> patched)
        : original(std::move(original)),
          patched(std::move(patched)) {}

    const std::vector<unsigned char> original; ///< Original bytes
    const std::vector<unsigned char> patched;  ///< Patched bytes
};
