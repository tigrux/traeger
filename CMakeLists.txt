cmake_minimum_required(VERSION 3.24)
project(traeger C CXX)
set(version 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
)
    add_compile_options(-Wall -Werror)
endif()

include(FetchContent)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(dependencies)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(traeger)

option(BUILD_PYTHON "Build python bindings" OFF)
if(BUILD_PYTHON)
    add_subdirectory(python)
endif()

find_program(GO go)
if(GO)
    add_subdirectory(go)
endif()

option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

export(
    TARGETS traeger_value traeger_format traeger_actor traeger_socket
    FILE traeger-targets.cmake
)

configure_file(
    ${traeger_SOURCE_DIR}/cmake/traeger-config.cmake.in
    ${traeger_BINARY_DIR}/traeger-config.cmake
    @ONLY
)

configure_file(
    ${traeger_SOURCE_DIR}/cmake/traeger-config-version.cmake.in
    ${traeger_BINARY_DIR}/traeger-config-version.cmake
    @ONLY
)

install(
    FILES
        ${traeger_BINARY_DIR}/traeger-config.cmake
        ${traeger_BINARY_DIR}/traeger-config-version.cmake
    DESTINATION lib/cmake/traeger
)

install(EXPORT traeger-targets DESTINATION lib/cmake/traeger)
