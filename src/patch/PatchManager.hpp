#pragma once

#include "../app/SublimeApp.hpp"
#include "../core/Architecture.hpp"
#include "../core/Platform.hpp"
#include "Patch.hpp"

#include <map>

using PatchCollection = std::vector<Patch>;
using ArchPatchMap = std::map<Architecture, PatchCollection>;
using PlatformPatchMap = std::map<Platform, ArchPatchMap>;

class PatchManager {
public:
    /// Load all available patches
    static void initPatches(PatchManager &manager);
    /// Check if the executable is already patched
    bool isAlreadyPatched(const SublimeApp &app) const;
    /// Apply patches to the SublimeApp instance
    bool applyPatches(SublimeApp &app) const;

private:
    /// Add a new patch for a specific Sublime application
    void addPatch(AppType app_type, Platform platform, Architecture arch, Patch patch);
    /// Add a new patch for Sublime Text
    void addSublimeTextPatch(Platform platform, Architecture arch, Patch patch);
    /// Add a new patch for Sublime Merge
    void addSublimeMergePatch(Platform platform, Architecture arch, Patch patch);
    /// Get all patches for a specific Sublime application
    std::optional<std::reference_wrapper<const PatchCollection>> getPatches(const SublimeApp &app) const;
    /// Get all patches for Sublime Text for a specific platform and architecture
    std::optional<std::reference_wrapper<const PatchCollection>> getSublimeTextPatches(Platform platform, Architecture arch) const;
    /// Get all patches for Sublime Merge for a specific platform and architecture
    std::optional<std::reference_wrapper<const PatchCollection>> getSublimeMergePatches(Platform platform, Architecture arch) const;

    PlatformPatchMap sublimeTextPatches;
    PlatformPatchMap sublimeMergePatches;
};
