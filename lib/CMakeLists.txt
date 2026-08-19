add_subdirectory(utils)

add_library(jlox
    code_providers/SourceCode.hh
    code_providers/SourceCode.cc
    code_providers/FileSourceReader.hh
    code_providers/FileSourceReader.cc
    code_providers/StringSourceReader.hh
    code_providers/StringSourceReader.cc
    Scanner.hh
    Scanner.cc
    Token.hh
    Token.cc
    TokenType.hh
)
target_include_directories(jlox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(jlox PRIVATE utils)
