#include <Scanner.hh>
#include <code_providers/SourceCode.hh>
#include <code_providers/StringSourceReader.hh>

#include <print>

int main(const int argc, const char *argv[])
{
    auto source_factory = code_provider::StringSourceReaderFactory::Create(" {  }()/+-*");
    auto source_provider = code_provider::SourceCodeProvider(std::move(source_factory));
    auto scanner = scanner::Scanner{std::move(source_provider)};
    for (const auto &token : scanner)
    {
        std::println("{}", token.ToString());
    }
    return 0;
}
