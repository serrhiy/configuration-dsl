add_library(utils INTERFACE utils.hh CircularBuffer.hh)
target_include_directories(utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_options(utils INTERFACE -freflection)
