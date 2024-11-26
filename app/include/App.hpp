#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace dynpax
{
namespace fs = std::filesystem;

struct App
{

    struct Params
    {
        fs::path target;
        fs::path fakeRoot;
#ifdef __linux__
        bool includeInterpreter;
#endif
    };
    using Result = std::expected<Params, std::runtime_error>;

    explicit App(std::string_view name);

    App(const App &) = delete;

    auto operator=(const App &) -> App & = delete;

    App(App &&) = delete;

    auto operator=(App &&) -> App & = delete;

    ~App();
    auto parse(int argc, char* argv[]) noexcept -> Result; // NOLINT

  private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
} // namespace dynpax