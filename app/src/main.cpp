#include "App.hpp"
#include "Executable.hpp"
#include "LibraryResolver.hpp"
#include <fmt/base.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>

auto main(int argc, char *argv[]) -> int
{
    dynpax::App app{"dynpax"};
    auto result = app.parse(argc, argv);
    if (!result)
    {
        fmt::println("Error: {}", result.error().what());
        return 1;
    }
    dynpax::Executable binary{result->target.string()};
    if (!binary)
    {
        fmt::println("Error: unable to open {}\n",
                     result->target.string());
        return 1;
    }
    for (const auto &library : binary.neededLibraries())
    {
        if (auto real =
                dynpax::LibraryResolver::resolveLibrary(library);
            real)
        {
            fmt::println("Needed {} => {}", library, real.value());
        }
    }

    return 0;
}