cmake_minimum_required(VERSION 3.10)

project(luaugame LANGUAGES C CXX)

option(LUAU_EXTERN_C "extern c" ON)

add_subdirectory(vendor/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/raylib)

add_executable(luaugame sources/main.c)

set_target_properties(luaugame PROPERTIES LINKER_LANGUAGE C)

if(WEB_DEMO)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${BUNDLE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  target_link_options(luaugame PRIVATE --preload-file ${BUNDLE})
endif()

target_link_libraries(luaugame PRIVATE Luau.Compiler Luau.VM raylib)
