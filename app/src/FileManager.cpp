#include "FileManager.hpp"
#include <filesystem>
#include <initializer_list>
#include <numeric>
#include <utility>

namespace dynpax
{
FileManager::FileManager(fs::path fakeRoot)
    : m_fakeRoot{std::move(fakeRoot)}
{
}

void FileManager::fakeRoot(fs::path path)
{
    m_fakeRoot = std::move(path);
}

auto FileManager::fakeRoot() const -> fs::path
{
    return m_fakeRoot;
}

auto FileManager::joinFakeRoot(
    std::initializer_list<fs::path> paths) const -> fs::path
{
    return std::accumulate(paths.begin(), paths.end(), m_fakeRoot,
                           [](const auto &left, const auto &right) {
                               return left /
                                      right.lexically_relative("/");
                           });
}

auto FileManager::copyFile(const fs::path &src, const fs::path &dst)
    -> bool
{
    if (!fs::exists(dst.parent_path()))
    {
        fs::create_directories(dst.parent_path());
    }
    return fs::copy_file(src, dst,
                         fs::copy_options::overwrite_existing |
                             fs::copy_options::copy_symlinks |
                             fs::copy_options::recursive);
}
} // namespace dynpax