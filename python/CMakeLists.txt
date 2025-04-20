if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.24)
    project(traeger-python CXX)
    set(version 0.1.0)
    set(CMAKE_CXX_STANDARD 17)

    find_package(traeger REQUIRED)
    include(FetchContent)
endif()

find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)

# robin-map
find_package(tsl-robin-map QUIET)
if(NOT tsl-robin-map_FOUND)
    FetchContent_Declare(
        tsl-robin-map
        URL https://github.com/Tessil/robin-map/archive/refs/tags/v1.4.0.tar.gz
        URL_HASH
            SHA256=7930dbf9634acfc02686d87f615c0f4f33135948130b8922331c16d90a03250c
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(tsl-robin-map)
endif()
find_package(tsl-robin-map REQUIRED)

# nanobind
find_package(nanobind QUIET)
if(NOT nanobind_FOUND)
    option(NB_TEST "Compile nanobind tests?" OFF)
    option(
        NB_USE_SUBMODULE_DEPS
        "Use the nanobind dependencies shipped as a git submodule of this repository"
        OFF
    )
    FetchContent_Declare(
        nanobind
        URL https://github.com/wjakob/nanobind/archive/refs/tags/v2.7.0.tar.gz
        URL_HASH
            SHA256=6c8c6bf0435b9d8da9312801686affcf34b6dbba142db60feec8d8e220830499
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(nanobind)
endif()
find_package(nanobind REQUIRED)

add_subdirectory(traeger)

if(CALL_FROM_SETUP_PY)
    install(FILES traeger/__init__.py DESTINATION traeger)
    install(TARGETS traeger_python DESTINATION traeger)
else()
    message(AUTHOR_WARNING "The Python bindings should be installed via pip")
    execute_process(
        COMMAND ${Python_EXECUTABLE} -m site --user-site
        OUTPUT_VARIABLE PYTHON_USER_SITE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        install(
            FILES traeger/__init__.py
            DESTINATION ${PYTHON_USER_SITE}/traeger
        )
        install(TARGETS traeger_python DESTINATION ${PYTHON_USER_SITE}/traeger)
    endif()
endif()
