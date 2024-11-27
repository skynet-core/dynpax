#include "App.hpp"
#include "Executable.hpp"
#include "FileManager.hpp"
#include "LibraryResolver.hpp"
#include <expected>
#include <fmt/base.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>
#include <stdexcept>
#include <string>

auto main(int argc, char *argv[]) -> int
{
    dynpax::App app{"dynpax"};
    auto parseResult = app.parse(argc, argv);
    if (!parseResult)
    {
        return parseResult.error();
    }
    dynpax::FileManager fileManager{std::move(parseResult->fakeRoot)};
    dynpax::Executable binary{parseResult->target.string()};
    if (!binary)
    {
        fmt::println("Error: unable to open {}\n",
                     parseResult->target.string());
        return 1;
    }
    fmt::println("Copy dynamic dependencies to {}",
                 fileManager.fakeRoot().string());
    for (const auto &library : binary.neededLibraries())
    {
        if (auto real =
                dynpax::LibraryResolver::resolveLibrary(library);
            real)
        {
            auto dst = fileManager.joinFakeRoot({real.value()});
            fmt::println("Copying {} => {}", real.value(),
                         dst.string());
            if (!dynpax::FileManager::copyFile(*real, dst))
            {
                fmt::println("Error: failed to copy: {} => {}", *real,
                             dst.string());
            }
        }
    }
    if (parseResult->includeInterpreter)
    {
        // Example of very easy to read and follow functional style
        // programming using C++23, enjoy! Ha-ha:)
        if (auto interpreterResult =
                binary.interpreter()
                    .and_then([](auto &&optInterpreter)
                                  -> std::expected<
                                      std::string,
                                      std::runtime_error> {
                        return optInterpreter
                            .transform([](auto &&value)
                                           -> std::expected<
                                               std::string,
                                               std::runtime_error> {
                                return value;
                            })
                            .value_or(
                                std::unexpected(std::runtime_error{
                                    "binary does not contain "
                                    "interpreter section to read"}))
                            .value();
                    })
                    .and_then([&fileManager](const auto &src)
                                  -> std::expected<
                                      bool, std::runtime_error> {
                        const auto dst =
                            fileManager.joinFakeRoot({src});
                        fmt::println("Copy {} => {}", src,
                                     dst.string());
                        return dynpax::FileManager::copyFile(src, dst)
                                   ? std::expected<
                                         bool,
                                         std::runtime_error>{true}
                                   : std::unexpected(
                                         std::runtime_error{
                                             "failed to copy "
                                             "interpreter: "
                                             "filesystem error"});
                    });
            !interpreterResult)
        {
            fmt::println("Error: {}",
                         interpreterResult.error().what());
            return 1;
        }
    }

    const auto binDst =
        fileManager.joinFakeRoot({"bin", binary.filePath()});
    fmt::println("Copy binary {} => {}", binary.filePath().string(),
                 binDst.string());
    if (!dynpax::FileManager::copyFile(binary.filePath(), binDst))
    {
        fmt::println("Error: failed to copy binary to new root");
        return 1;
    }

    return 0;
}