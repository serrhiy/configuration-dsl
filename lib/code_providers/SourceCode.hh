#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <string>

namespace code_provider
{

struct SourceChar
{
    char symbol;
    std::size_t line;
    std::size_t column;
};

class ISourceReader
{
  public:
    virtual ~ISourceReader() = default;
    virtual std::optional<char> ReadSymbol() = 0;
};

class ISourceReaderFactory
{
  public:
    virtual ~ISourceReaderFactory() = default;
    virtual std::unique_ptr<ISourceReader> CreateSourceReader() = 0;
};

class SourceCode
{
    std::unique_ptr<ISourceReaderFactory> source_reader_factory;

    class Iterator
    {
        std::unique_ptr<ISourceReader> source_reader;
        std::size_t line;
        std::size_t column;
        std::optional<SourceChar> current_char;

        void Pull();

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = SourceChar;

        explicit Iterator(std::unique_ptr<ISourceReader> source_reader);

        const value_type &operator*() const;
        const value_type *operator->() const;
        Iterator &operator++();
        void operator++(int);
        bool operator==(std::default_sentinel_t sentinel) const;
    };

    static_assert(std::input_iterator<Iterator>);

  public:
    explicit SourceCode(std::unique_ptr<ISourceReaderFactory> source_reader_factory);

    Iterator begin() const;
    std::default_sentinel_t end() const;
};

static_assert(std::ranges::input_range<SourceCode>);

} // namespace code_provider
