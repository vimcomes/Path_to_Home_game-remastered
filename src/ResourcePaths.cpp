#include "ResourcePaths.h"
#include <filesystem>

namespace
{
std::filesystem::path exeDir() {
    static std::filesystem::path base;
    if (!base.empty())
        return base;

    std::error_code ec;
    std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", ec);
    if (ec)
        exe = std::filesystem::current_path();
    base = exe.parent_path();
    return base;
}
} // namespace

std::string resourcePath(std::string_view relative) {
    return (exeDir() / "resources" / relative).string();
}

std::string executableRelative(std::string_view relative) {
    return (exeDir() / relative).string();
}
