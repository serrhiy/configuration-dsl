#include "Token.hh"
#include "utils/utils.hh"

#include <format>
#include <meta>
#include <string>
#include <string_view>

using namespace tokenizer;

namespace
{

std::string_view LiteralToString(const Literal &literal)
{
    struct LiteralVisitor
    {
        std::string_view operator()(std::monostate) const
        {
            return "nil";
        }
        std::string_view operator()(double) const
        {
            return "number";
        }
        std::string_view operator()(const std::string &) const
        {
            return "string";
        }
    };
    return std::visit(LiteralVisitor{}, literal);
}

} // namespace

std::string Token::ToString() const
{
    static constexpr std::string_view format = "type={}; lexeme={}; literal={}; column={}";

    std::string_view literal_string = LiteralToString(literal);
    std::string_view enum_name = utils::GetEnumName(type);
    return std::format(format, enum_name, lexeme, literal_string, column);
}