cmake_minimum_required(VERSION 3.24)
project(traeger C CXX)
set(version 0.2.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(!WIN32)
    add_compile_options(-Wall -Werror)
endif()

include(FetchContent)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(dependencies)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(traeger)

option(BUILD_PYTHON "Build Python bindings" OFF)
if(BUILD_PYTHON)
    add_subdirectory(python)
endif()

option(BUILD_GO "Build Go bindings" OFF)
if(BUILD_GO)
    find_program(GO go)
    if(GO)
        add_subdirectory(go)
    endif()
endif()

option(BUILD_RUST "Build Rust bindings" OFF)
if(BUILD_RUST)
    find_program(RUSTC rustc)
    if(RUSTC)
        add_subdirectory(rust)
    endif()
endif()

option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

export(TARGETS ${TRAEGER_LIBRARIES} FILE traeger-targets.cmake)

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
