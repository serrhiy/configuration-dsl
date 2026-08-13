#pragma once

#include <string>
#include <variant>

#include "TokenType.hh"

namespace tokenizer
{

using Literal = std::variant<std::monostate, double, std::string>;

struct Token
{
    TokenType type;
    std::string lexeme;
    Literal literal;
    std::size_t line;

    std::string ToString() const;
};

} // namespace tokenizer