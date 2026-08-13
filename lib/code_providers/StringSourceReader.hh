#pragma once

#include "SourceCode.hh"

#include <memory>
#include <string>

namespace code_provider
{

class StringSourceReader final : public ISourceReader
{
    std::string source_code;
    std::size_t index;

  public:
    explicit StringSourceReader(std::string source_code);
    static std::unique_ptr<StringSourceReader> Create(std::string source_code);

    std::optional<char> ReadSymbol() override;
};

class StringSourceReaderFactory final : public ISourceReaderFactory
{
    std::string source_code;

  public:
    explicit StringSourceReaderFactory(std::string source_code);
    static std::unique_ptr<StringSourceReaderFactory> Create(std::string source_code);

    std::unique_ptr<ISourceReader> CreateSourceReader() override;
};

} // namespace code_provider