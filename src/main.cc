#include <code_providers/SourceCode.hh>
#include <code_providers/StringSourceReader.hh>
#include <print>

int main(const int argc, const char *argv[])
{
    auto source_factory = code_provider::StringSourceReaderFactory::Create("Hello1\nHello2");
    auto source_provider = code_provider::SourceCode(std::move(source_factory));
    for (const auto symbol : source_provider)
    {
        std::println("line={}, column={}, symbol={}", symbol.line, symbol.column, symbol.symbol);
    }
    for (const auto symbol : source_provider)
    {
        std::println("line={}, column={}, symbol={}", symbol.line, symbol.column, symbol.symbol);
    }
    return 0;
}
