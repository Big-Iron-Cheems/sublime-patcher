#include "PatchManager.hpp"

void PatchManager::initPatches(PatchManager &manager) {
    /*
     * mov eax, edi (original instruction)
     * add rsp, 0x238
     *
     * xor eax, eax (patch: always return 0)
     * Unchanged bytes
     *
     * Offset 0x80559
     */
    manager.addPatch(AppType::SublimeText, Platform::Windows, Architecture::x64, Patch("E8 ?? ?? ?? ?? 89 F8 48 81 C4 38 02 00 00", "E8 ?? ?? ?? ?? 31 C0 48 81 C4 38 02 00 00"));

    /*
     * mov eax, r13d (original instruction)
     * add rsp, 0x2428
     *
     * xor eax, eax + NOP (patch: always return 0)
     * Unchanged bytes
     *
     * Offset 0x3FE0D2
     */
    manager.addPatch(AppType::SublimeText, Platform::Linux, Architecture::x64,
                     Patch("E8 ?? ?? ?? ?? 44 89 E8 48 81 C4 28 24 00 00", "E8 ?? ?? ?? ?? 31 C0 90 48 81 C4 28 24 00 00"));

    /*
     * mov eax, r13d (original instruction)
     * add rsp, 0x218
     *
     * xor eax, eax + NOP (patch: always return 0)
     * Unchanged bytes
     *
     * Offset 0x9F959
     */
    manager.addPatch(AppType::SublimeText, Platform::macOS, Architecture::x64,
                     Patch("0F ?? ?? ?? ?? ?? 44 89 E8 48 81 C4 18 02 00 00", "0F ?? ?? ?? ?? ?? 31 C0 90 48 81 C4 18 02 00 00"));

    /*
     * mov eax, ebp (original instruction)
     * add rsp, 0x258
     *
     * xor eax, eax (patch: always return 0)
     * Unchanged bytes
     *
     * Offset 0x1CF37
     */
    manager.addPatch(AppType::SublimeMerge, Platform::Windows, Architecture::x64, Patch("E8 ?? ?? ?? ?? 89 E8 48 81 C4 58 02 00 00", "E8 ?? ?? ?? ?? 31 C0 48 81 C4 58 02 00 00"));

    /*
     * mov eax, r14d (original instruction)
     * add rsp, 0x2448
     *
     * xor eax, eax + NOP (patch: always return 0)
     * Unchanged bytes
     *
     * Offset 0x494AD7
     */
    manager.addPatch(AppType::SublimeMerge, Platform::Linux, Architecture::x64,
                     Patch("E8 ?? ?? ?? ?? 44 89 F0 48 81 C4 48 24 00 00", "E8 ?? ?? ?? ?? 31 C0 90 48 81 C4 48 24 00 00"));

    /*
     * mov eax, r15d (original instruction)
     * add rsp, 0x258
     *
     * xor eax, eax + NOP (patch: always return 0)
     * Unchanged bytes
     *
     * Offset 0xB0371
     */
    manager.addPatch(AppType::SublimeMerge, Platform::macOS, Architecture::x64,
                     Patch("0F ?? ?? ?? ?? ?? 44 89 F8 48 81 C4 58 02 00 00", "0F ?? ?? ?? ?? ?? 31 C0 90 48 81 C4 58 02 00 00"));
}

void PatchManager::addPatch(const AppType app_type, const Platform platform, const Architecture arch, Patch patch) {
    app_type == AppType::SublimeText ? addSublimeTextPatch(platform, arch, std::move(patch)) : addSublimeMergePatch(platform, arch, std::move(patch));
}

void PatchManager::addSublimeTextPatch(const Platform platform, const Architecture arch, Patch patch) { sublimeTextPatches[platform][arch].push_back(std::move(patch)); }

void PatchManager::addSublimeMergePatch(const Platform platform, const Architecture arch, Patch patch) { sublimeMergePatches[platform][arch].push_back(std::move(patch)); }

bool PatchManager::isAlreadyPatched(const SublimeApp &app) const {
    const auto patches = getPatches(app);
    if (!patches) return false;

    size_t found_patches = 0;
    const size_t total_patches = patches->get().size();

    for (const auto &patch: patches->get()) {
        // All non-wildcard bytes in the patched pattern match, so this patch is applied
        if (Patch::findPatternInBytes(app.file_data, patch.patched)) found_patches++;
    }

    if (found_patches == total_patches) {
        spdlog::info("[+] The executable is already fully patched ({}/{} patches)", found_patches, total_patches);
        return true;
    }

    if (found_patches == 0) {
        spdlog::info("[*] The executable has no patches applied (0/{} patches)", total_patches);
    } else {
        spdlog::info("[*] The executable is partially patched ({}/{} patches)", found_patches, total_patches);
    }
    return false;
}

bool PatchManager::applyPatches(SublimeApp &app) const {
    const auto patches = getPatches(app);
    if (!patches) return false;

    const size_t total_patches = patches->get().size();
    size_t applied_patches = 0;

    for (const auto &patch: patches->get()) {
        auto offset_opt = Patch::findPatternInBytes(app.patched_data, patch.original);
        if (!offset_opt) {
            spdlog::warn("[-] Could not find pattern to patch");
            continue;
        }

        size_t offset = *offset_opt;
        spdlog::info("[*] Found pattern at offset: 0x{:X}", offset);

        // Apply the patch to patched_data (replace non-wildcard bytes)
        bool changed = false;
        for (size_t i = 0; i < patch.patched.size(); ++i) {
            if (!patch.patched[i].isWildcard && app.patched_data[offset + i] != patch.patched[i].value) {
                spdlog::info("[*] Replacing byte at offset 0x{:X}: 0x{:02X} -> 0x{:02X}", offset + i, app.patched_data[offset + i], patch.patched[i].value);
                app.patched_data[offset + i] = patch.patched[i].value;
                changed = true;
            }
        }

        if (changed) {
            applied_patches++;
            spdlog::info("[+] Successfully applied patch {}/{}", applied_patches, total_patches);
        } else {
            spdlog::info("[*] Patch already applied or no changes needed");
        }
    }

    if (applied_patches == total_patches) {
        spdlog::info("[+] Successfully applied all patches ({}/{} patches)", applied_patches, total_patches);
        return true;
    }

    spdlog::warn("[-] Failed to apply all patches ({}/{} patches)", applied_patches, total_patches);
    return false;
}

std::optional<std::reference_wrapper<const PatchCollection>> PatchManager::getPatches(const SublimeApp &app) const {
    return app.app_type == AppType::SublimeText ? getSublimeTextPatches(app.app_info.platform, app.app_info.arch)
                                                : getSublimeMergePatches(app.app_info.platform, app.app_info.arch);
}

std::optional<std::reference_wrapper<const PatchCollection>> PatchManager::getSublimeTextPatches(const Platform platform, const Architecture arch) const {
    if (sublimeTextPatches.contains(platform) && sublimeTextPatches.at(platform).contains(arch)) return std::ref(sublimeTextPatches.at(platform).at(arch));
    spdlog::warn("[-] Warning: Could not find SublimeText patches for platform: {} and architecture: {}", platform, arch);
    return std::nullopt;
}

std::optional<std::reference_wrapper<const PatchCollection>> PatchManager::getSublimeMergePatches(const Platform platform, const Architecture arch) const {
    if (sublimeMergePatches.contains(platform) && sublimeMergePatches.at(platform).contains(arch)) return std::ref(sublimeMergePatches.at(platform).at(arch));
    spdlog::warn("[-] Warning: Could not find SublimeMerge patches for platform: {} and architecture: {}", platform, arch);
    return std::nullopt;
}
