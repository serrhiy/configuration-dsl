#include "Scanner.hh"
#include "Token.hh"
#include "TokenType.hh"
#include "code_providers/SourceCode.hh"

#include <cctype>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace scanner;
using namespace code_provider;

namespace
{

const std::unordered_map<std::string_view, tokenizer::TokenType> LEXEMES{
    {"(", tokenizer::TokenType::LEFT_PAREN}, {")", tokenizer::TokenType::RIGHT_PAREN},
    {"{", tokenizer::TokenType::LEFT_BRACE}, {"}", tokenizer::TokenType::RIGHT_BRACE},
    {",", tokenizer::TokenType::COMMA},      {".", tokenizer::TokenType::DOT},
    {"-", tokenizer::TokenType::MINUS},      {"+", tokenizer::TokenType::PLUS},
    {";", tokenizer::TokenType::SEMICOLON},  {"/", tokenizer::TokenType::SLASH},
    {"*", tokenizer::TokenType::STAR},
};

constexpr size_t MAX_LEXEME_SIZE = 16;

} // namespace

Scanner::Scanner(SourceCodeProvider source_code_provider)
    : source_code_provider{std::move(source_code_provider)}
{
}

Scanner::Iterator Scanner::begin() const
{
    return Iterator{source_code_provider.begin()};
}

std::default_sentinel_t Scanner::end() const
{
    return std::default_sentinel;
}

void Scanner::Iterator::Pull()
{
    std::string lexeme;
    lexeme.reserve(MAX_LEXEME_SIZE);
    while (iterator != std::default_sentinel)
    {
        const auto symbol = *iterator;
        ++iterator;
        if (std::isspace(symbol.symbol))
        {
            continue;
        }
        lexeme.push_back(symbol.symbol);
        if (LEXEMES.contains(lexeme))
        {
            current_token = tokenizer::Token{.type = LEXEMES.at(lexeme),
                                             .lexeme = std::move(lexeme),
                                             .literal = std::monostate{},
                                             .line = symbol.line,
                                             .column = symbol.column};
            return;
        }
    }
    current_token.reset();
}

Scanner::Iterator::Iterator(SourceCodeProvider::Iterator iterator) : iterator{std::move(iterator)}
{
    Pull();
}

const Scanner::Iterator::value_type &Scanner::Iterator::operator*() const
{
    return current_token.value();
}

const Scanner::Iterator::value_type *Scanner::Iterator::operator->() const
{
    return &this->operator*();
}

Scanner::Iterator &Scanner::Iterator::operator++()
{
    Pull();
    return *this;
}

void Scanner::Iterator::operator++(int)
{
    Pull();
}

bool Scanner::Iterator::operator==(std::default_sentinel_t sentinel) const
{
    return !current_token.has_value();
}
