#include "FileSourceReader.hh"

#include <filesystem>
#include <memory>

using namespace code_provider;

FileSourceReader::FileSourceReader(std::filesystem::path source_path) : source_path{std::move(source_path)}
{
}

std::unique_ptr<FileSourceReader> FileSourceReader::Create(std::filesystem::path source_path)
{
    return std::make_unique<FileSourceReader>(std::move(source_path));
}

std::optional<char> FileSourceReader::ReadSymbol()
{
    if (source_file.fail() || source_file.eof())
    {
        return std::nullopt;
    }

    if (!source_file.is_open())
    {
        source_file.exceptions(std::ios::badbit | std::ios::failbit);
        source_file.open(source_path);
        source_file.exceptions(std::ios::badbit);
    }

    const int symbol = source_file.get();
    if (symbol == EOF)
    {
        return std::nullopt;
    }
    return static_cast<char>(symbol);
}
