cmake_minimum_required(VERSION 3.10)

project(luaugame LANGUAGES C CXX)

option(LUAU_EXTERN_C "extern c" ON)

add_subdirectory(vendor/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/raylib)

add_executable(luaugame sources/main.c)

set_target_properties(luaugame PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(luaugame PRIVATE Luau.Compiler Luau.VM raylib)
