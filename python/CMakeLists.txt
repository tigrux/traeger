if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.24)
    project(traeger-python CXX)
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
        GIT_REPOSITORY https://github.com/Tessil/robin-map/
        GIT_TAG v1.4.0
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
        GIT_REPOSITORY https://github.com/wjakob/nanobind
        GIT_TAG v2.7.0
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
    install(TARGETS traeger DESTINATION traeger)
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
        install(TARGETS traeger DESTINATION ${PYTHON_USER_SITE}/traeger)
    endif()
endif()
