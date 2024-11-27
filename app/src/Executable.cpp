#include "Executable.hpp"
#include "LIEF/Abstract/Parser.hpp"
#include "LIEF/ELF/Binary.hpp"
#include "LIEF/ELF/DynamicEntry.hpp"
#include "LIEF/ELF/DynamicEntryRunPath.hpp"
#include <LIEF/ELF.hpp>
#include <LIEF/LIEF.hpp>
#include <LIEF/Object.hpp>
#include <algorithm>
#include <expected>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace dynpax
{

struct Executable::Impl
{
    explicit Impl(std::string name)
        : binary{LIEF::Parser::parse(name)},
          filePath_{std::move(name)}
    {
    }

    explicit operator bool() const
    {
        return binary != nullptr;
    }

    [[nodiscard]] auto neededLibraries() const
    {
        return binary->imported_libraries();
    }

    [[nodiscard]] auto interpreter() const
        -> std::expected<std::optional<std::string>,
                         std::runtime_error>
    {
        if (LIEF::ELF::Binary::classof(binary.get()))
        {
            // NOLINTNEXTLINE
            auto &elf = static_cast<LIEF::ELF::Binary &>(*binary);
            if (!elf.has_interpreter())
            {
                return std::optional<std::string>{};
            }
            return elf.interpreter();
        }
        return std::unexpected{std::runtime_error{"not ELF binary"}};
    }

    [[nodiscard]] auto runpath() const
        -> std::expected<std::optional<std::vector<std::string>>,
                         std::runtime_error>
    {
        if (LIEF::ELF::Binary::classof(binary.get()))
        {
            // NOLINTNEXTLINE
            auto &elf = static_cast<LIEF::ELF::Binary &>(*binary);
            for (auto &entry : elf.dynamic_entries())
            {
                if (entry.tag() ==
                    LIEF::ELF::DynamicEntry::TAG::RUNPATH)
                {
                    // NOLINTNEXTLINE
                    return static_cast<
                               LIEF::ELF::DynamicEntryRunPath &>(
                               entry)
                        .paths();
                }
            }
            return std::optional<std::vector<std::string>>{};
        }
        return std::unexpected{std::runtime_error{"not ELF binary"}};
    }

    void interpreter(const std::string &name)
    {
        if (LIEF::ELF::Binary::classof(binary.get()))
        {
            auto &elf =
                static_cast<LIEF::ELF::Binary &>(*binary); // NOLINT
            elf.interpreter(name);
        }
    }

    void runpath(const std::vector<std::string> &runpath)
    {
        if (LIEF::ELF::Binary::classof(binary.get()))
        {
            auto &elf =
                static_cast<LIEF::ELF::Binary &>(*binary); // NOLINT
            LIEF::ELF::DynamicEntryRunPath entry{runpath};
            elf.add(entry);
        }
    }

    auto write(const fs::path &destFile) -> bool
    {
        try
        {
            if (destFile.has_parent_path() &&
                !fs::exists(destFile.parent_path()))
            {
                fs::create_directories(destFile.parent_path());
            }
            binary->write(destFile.string());
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    [[nodiscard]] auto filePath() const -> fs::path
    {
        return filePath_;
    }

  private:
    decltype(LIEF::Parser::parse("")) binary{nullptr};
    fs::path filePath_;
};

Executable::Executable(std::string name)
    : pimpl{std::make_unique<Impl>(std::move(name))}
{
}

Executable::Executable(Executable &&rhs) noexcept
{
    if (this != &rhs)
    {
        swap(rhs);
    }
}

auto Executable::operator=(Executable &&rhs) noexcept -> Executable &
{
    if (this == &rhs)
    {
        return *this;
    }
    swap(rhs);
    return *this;
}

Executable::~Executable() = default;

void Executable::swap(Executable &rhs) noexcept
{
    std::swap(this->pimpl, rhs.pimpl);
}

Executable::operator bool() const
{
    return bool(pimpl);
}

[[nodiscard]] auto Executable::interpreter() const
    -> std::expected<std::optional<std::string>, std::runtime_error>
{
    return pimpl->interpreter();
}

[[nodiscard]] auto Executable::neededLibraries() const
    -> std::vector<std::string>
{
    return pimpl->neededLibraries();
}

[[nodiscard]] auto Executable::runpath() const
    -> std::expected<std::optional<std::vector<std::string>>,
                     std::runtime_error>
{
    return pimpl->runpath();
}

auto Executable::write(const fs::path &destFile) -> bool
{
    return pimpl->write(destFile);
}

void Executable::interpreter(const std::string &interpreter)
{
    pimpl->interpreter(interpreter);
}

void Executable::runpath(const std::vector<std::string> &runpath)
{
    pimpl->runpath(runpath);
}

[[nodiscard]] auto Executable::filePath() const -> fs::path
{
    return pimpl->filePath();
}

void swap(Executable &lhs, Executable &rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace dynpax