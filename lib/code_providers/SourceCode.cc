#include "SourceCode.hh"

#include <iterator>
#include <memory>
#include <optional>

using namespace code_provider;

SourceCode::SourceCode(std::unique_ptr<ISourceReaderFactory> source_reader_factory)
    : source_reader_factory{std::move(source_reader_factory)}
{
}

SourceCode::Iterator SourceCode::begin() const
{
    return Iterator{source_reader_factory->CreateSourceReader()};
}

std::default_sentinel_t SourceCode::end() const
{
    return std::default_sentinel;
}

void SourceCode::Iterator::Pull()
{
    const auto symbol = source_reader->ReadSymbol();
    if (!symbol.has_value())
    {
        current_char.reset();
        return;
    }
    current_char = SourceChar{symbol.value(), line, column};
    if (symbol.value() == '\n')
    {
        column = 1;
        line++;
    }
    else
    {
        column++;
    }
}

SourceCode::Iterator::Iterator(std::unique_ptr<ISourceReader> source_reader)
    : source_reader{std::move(source_reader)}, line{1}, column{1}, current_char{std::nullopt}
{
    Pull();
}

const SourceCode::Iterator::value_type &SourceCode::Iterator::operator*() const
{
    return current_char.value();
}

const SourceCode::Iterator::value_type *SourceCode::Iterator::operator->() const
{
    return &this->operator*();
}

SourceCode::Iterator &SourceCode::Iterator::operator++()
{
    Pull();
    return *this;
}

void SourceCode::Iterator::operator++(int)
{
    Pull();
}

bool code_provider::SourceCode::Iterator::operator==(std::default_sentinel_t sentinel) const
{
    return !current_char.has_value();
}
