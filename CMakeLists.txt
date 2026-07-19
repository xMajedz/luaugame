cmake_minimum_required(VERSION 3.10)
project(luaugame LANGUAGES CXX)

add_subdirectory(vendor/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/raylib)

add_executable(luaugame sources/main.cpp)

target_link_libraries(luaugame PRIVATE Luau.Compiler Luau.VM raylib)
