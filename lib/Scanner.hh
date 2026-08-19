#pragma once

#include "Token.hh"
#include "code_providers/SourceCode.hh"

#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>

namespace scanner
{

class Scanner
{
    code_provider::SourceCodeProvider source_code_provider;

    class Iterator
    {
        code_provider::SourceCodeProvider::Iterator iterator;
        std::optional<tokenizer::Token> current_token;

        void Pull();

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = tokenizer::Token;

        explicit Iterator(code_provider::SourceCodeProvider::Iterator iterator);

        const value_type &operator*() const;
        const value_type *operator->() const;

        Iterator &operator++();
        void operator++(int);
        bool operator==(std::default_sentinel_t sentinel) const;
    };

    static_assert(std::input_iterator<Iterator>);

  public:
    explicit Scanner(code_provider::SourceCodeProvider source_code_provider);

    Iterator begin() const;
    std::default_sentinel_t end() const;
};

static_assert(std::ranges::input_range<Scanner>);

}; // namespace scanner
