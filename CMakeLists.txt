cmake_minimum_required(VERSION 3.31...4.2)
project(configuration-dsl DESCRIPTION "Configuration DSL" LANGUAGES CXX)

option(CONFIGURATION_DSL_TESTS_ENABLE "Build unit tests" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(lib)
add_subdirectory(src)

if (CONFIGURATION_DSL_TESTS_ENABLE)
    enable_testing()
    add_subdirectory(tests)
endif()