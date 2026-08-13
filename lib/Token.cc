#include "Token.hh"

#include <format>
#include <meta>
#include <string>
#include <string_view>

using namespace tokenizer;

namespace
{

template <typename E>
    requires(std::meta::is_enum_type(^^E))
constexpr std::string_view GetEnumName(E value)
{
    constexpr static auto enumerators = std::define_static_array(enumerators_of(^^E));
    template for (constexpr auto enumerator : enumerators)
    {
        if (value != [:enumerator:])
            continue;
        return std::meta::identifier_of(enumerator);
    }
    return "<unknown>";
}

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
    static constexpr std::string_view format = "type={}; lexeme={}; literal={}";

    std::string_view literal_string = LiteralToString(literal);
    return std::format(format, GetEnumName(type), lexeme, literal_string);
}