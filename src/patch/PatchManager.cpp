#include "PatchManager.hpp"

void PatchManager::initPatches(PatchManager &manager) {
    // Sublime Text patches
    manager.addSublimeTextPatch(Platform::Windows, Architecture::x64,
                                Patch(
                                        {
                                                0x89, 0xF8,                               // mov eax, edi (original instruction)
                                                0x48, 0x81, 0xC4, 0x38, 0x02, 0x00, 0x00, // add rsp, 0x238
                                                0x5B, 0x5D, 0x5F, 0x5E, 0x41, 0x5C, 0x41  // pop registers (function epilogue)
                                        },
                                        {
                                                0x31, 0xC0,                               // xor eax, eax (patch: always return 0)
                                                0x48, 0x81, 0xC4, 0x38, 0x02, 0x00, 0x00, // Unchanged bytes
                                                0x5B, 0x5D, 0x5F, 0x5E, 0x41, 0x5C, 0x41  // Unchanged bytes
                                        }));

    manager.addSublimeTextPatch(Platform::Linux, Architecture::x64,
                                Patch(
                                        {
                                                0x44, 0x89, 0xE8,                         // mov eax, r13d (original instruction)
                                                0x48, 0x81, 0xC4, 0x28, 0x24, 0x00, 0x00, // add rsp, 0x2428
                                                0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // pop registers (function epilogue)
                                        },
                                        {
                                                0x31, 0xC0, 0x90,                         // xor eax, eax + NOP (patch: always return 0)
                                                0x48, 0x81, 0xC4, 0x28, 0x24, 0x00, 0x00, // Unchanged bytes
                                                0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // Unchanged bytes
                                        }));

    manager.addSublimeTextPatch(Platform::MacOS, Architecture::x64,
                                Patch(
                                        {
                                                0x44, 0x89, 0xE8,                         // mov eax, r13d (original instruction)
                                                0x48, 0x81, 0xC4, 0x18, 0x02, 0x00, 0x00, // add rsp, 0x218
                                                0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // pop registers (function epilogue)
                                        },
                                        {
                                                0x31, 0xC0, 0x90,                         // xor eax, eax + NOP (patch: always return 0)
                                                0x48, 0x81, 0xC4, 0x18, 0x02, 0x00, 0x00, // Unchanged bytes
                                                0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // Unchanged bytes
                                        }));
    // Sublime Merge patches
    manager.addSublimeMergePatch(Platform::Windows, Architecture::x64,
                                 Patch(
                                         {
                                                 0x89, 0xE8,                               // mov eax, ebp (original instruction)
                                                 0x48, 0x81, 0xC4, 0x58, 0x02, 0x00, 0x00, // add rsp, 0x258
                                                 0x5B, 0x5D, 0x5F, 0x5E, 0x41, 0x5C, 0x41  // pop registers (function epilogue)
                                         },
                                         {
                                                 0x31, 0xC0,                               // xor eax, eax (patch: always return 0)
                                                 0x48, 0x81, 0xC4, 0x58, 0x02, 0x00, 0x00, // Unchanged bytes
                                                 0x5B, 0x5D, 0x5F, 0x5E, 0x41, 0x5C, 0x41  // Unchanged bytes
                                         }));

    manager.addSublimeMergePatch(Platform::Linux, Architecture::x64,
                                 Patch(
                                         {
                                                 0x44, 0x89, 0xF0,                         // mov eax, r14d
                                                 0x48, 0x81, 0xC4, 0x48, 0x24, 0x00, 0x00, // add rsp, 0x2448
                                                 0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // pop registers (function epilogue)
                                         },
                                         {
                                                 0x31, 0xC0, 0x90,                         // xor eax, eax + NOP (patch: always return 0)
                                                 0x48, 0x81, 0xC4, 0x48, 0x24, 0x00, 0x00, // Unchanged bytes
                                                 0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // Unchanged bytes
                                         }));

    manager.addSublimeMergePatch(Platform::MacOS, Architecture::x64,
                                 Patch(
                                         {
                                                 0x44, 0x89, 0xF8,                         // mov eax,r15d
                                                 0x48, 0x81, 0xC4, 0x58, 0x02, 0x00, 0x00, // add rsp,0x258
                                                 0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // pop registers (function epilogue)
                                         },
                                         {
                                                 0x31, 0xC0, 0x90,                         // xor eax, eax + NOP (patch: always return 0)
                                                 0x48, 0x81, 0xC4, 0x58, 0x02, 0x00, 0x00, // Unchanged bytes
                                                 0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41        // Unchanged bytes
                                         }));
}

void PatchManager::addSublimeTextPatch(const Platform platform, const Architecture arch, Patch patch) { sublimeTextPatches[platform][arch].push_back(std::move(patch)); }

void PatchManager::addSublimeMergePatch(const Platform platform, const Architecture arch, Patch patch) { sublimeMergePatches[platform][arch].push_back(std::move(patch)); }

bool PatchManager::isAlreadyPatched(const SublimeApp &app) const {
    try {
        for (const auto &patch: getPatches(app)) {
            if (std::ranges::search(app.file_data, patch.patched).begin() != app.file_data.end()) {
                spdlog::info("[+] The executable is already patched");
                return true;
            }
        }
    } catch (const std::runtime_error &e) {
        spdlog::error("[-] Error: {}", e.what());
    }
    return false;
}

bool PatchManager::applyPatches(SublimeApp &app) const {
    for (const auto &[original_bytes, patched_bytes]: getPatches(app)) {
        if (auto result = std::ranges::search(app.patched_data, original_bytes); result.begin() != result.end()) {
            std::ranges::copy(patched_bytes, result.begin());
            spdlog::info("[+] Applied patch at offset: 0x{:X}", std::distance(app.patched_data.begin(), result.begin()));
        } else {
            spdlog::error("[-] Error: Could not find original bytes to patch");
            return false;
        }
    }
    return true;
}

const PatchCollection &PatchManager::getPatches(const SublimeApp &app) const {
    if (app.app_type == AppType::SublimeText) return getSublimeTextPatches(app.app_info.platform, app.app_info.arch);
    return getSublimeMergePatches(app.app_info.platform, app.app_info.arch);
}

const PatchCollection &PatchManager::getSublimeTextPatches(const Platform platform, const Architecture arch) const {
    if (!sublimeTextPatches.contains(platform) || !sublimeTextPatches.at(platform).contains(arch)) {
        throw std::runtime_error(std::format("[-] Error: Could not find SublimeText patches for platform: {} and architecture: {}", platform, arch));
    }
    return sublimeTextPatches.at(platform).at(arch);
}

const PatchCollection &PatchManager::getSublimeMergePatches(const Platform platform, const Architecture arch) const {
    if (!sublimeMergePatches.contains(platform) || !sublimeMergePatches.at(platform).contains(arch)) {
        throw std::runtime_error(std::format("[-] Error: Could not find SublimeMerge patches for platform: {} and architecture: {}", platform, arch));
    }
    return sublimeMergePatches.at(platform).at(arch);
}
