add_executable(main main.cc)
target_link_libraries(main PRIVATE jlox)
install(TARGETS main RUNTIME DESTINATION .)