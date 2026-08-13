#pragma once

#include "SourceCode.hh"

#include <filesystem>
#include <fstream>
#include <memory>

namespace code_provider
{

class FileSourceReader final : public ISourceReader
{
    std::filesystem::path source_path;
    std::ifstream source_file;

  public:
    explicit FileSourceReader(std::filesystem::path source_path);
    static std::unique_ptr<FileSourceReader> Create(std::filesystem::path source_path);

    std::optional<char> ReadSymbol() override;
};

} // namespace code_provider