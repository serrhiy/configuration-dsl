#include "StringSourceReader.hh"

#include <string>

using namespace code_provider;

StringSourceReader::StringSourceReader(std::string source_code) : source_code{std::move(source_code)}, index{0}
{
}

std::unique_ptr<StringSourceReader> StringSourceReader::Create(std::string source_code)
{
    return std::make_unique<StringSourceReader>(std::move(source_code));
}

std::optional<char> StringSourceReader::ReadSymbol()
{
    if (index >= source_code.size())
    {
        return std::nullopt;
    }
    return source_code[index++];
}

StringSourceReaderFactory::StringSourceReaderFactory(std::string source_code) : source_code{std::move(source_code)}
{
}

std::unique_ptr<StringSourceReaderFactory> StringSourceReaderFactory::Create(std::string source_code)
{
    return std::make_unique<StringSourceReaderFactory>(std::move(source_code));
}

std::unique_ptr<ISourceReader> StringSourceReaderFactory::CreateSourceReader()
{
    return std::make_unique<StringSourceReader>(source_code);
}
