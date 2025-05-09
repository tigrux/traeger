# immer
find_package(Immer QUIET)
if(NOT Immer_FOUND)
    option(immer_BUILD_TESTS "Build immer tests" OFF)
    option(immer_BUILD_EXAMPLES "Build immer examples" OFF)
    option(immer_BUILD_DOCS "Build immer docs" ON)
    option(immer_BUILD_EXTRAS "Build immer extras" ON)
    FetchContent_Declare(
        immer
        GIT_REPOSITORY https://github.com/arximboldi/immer
        GIT_TAG v0.8.1
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(immer)
endif()
find_package(Immer REQUIRED)

# json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    option(JSON_BuildTests "Build json unit tests" OFF)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.12.0
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()
find_package(nlohmann_json REQUIRED)

# yaml
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    option(YAML_BUILD_SHARED_LIBS "Build yaml-cpp shared library" OFF)
    option(YAML_CPP_BUILD_TESTS "Enable yaml-cpp tests" OFF)
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        GIT_TAG 0.8.0
        PATCH_COMMAND
            git apply
            ${CMAKE_CURRENT_SOURCE_DIR}/patches/0001-Fix-missing-header-cstdint.patch
        UPDATE_DISCONNECTED 1
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(yaml-cpp)
endif()
find_package(yaml-cpp REQUIRED)

# msgpack
find_package(msgpack-cxx QUIET)
if(NOT msgpack-cxx_FOUND)
    option(MSGPACK_USE_BOOST "Use msgpack Boost libraried" OFF)
    option(MSGPACK_BUILD_DOCS "Build msgpack Doxygen documentation" OFF)
    FetchContent_Declare(
        msgpack-cxx
        GIT_REPOSITORY https://github.com/msgpack/msgpack-c
        GIT_TAG cpp-7.0.0
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(msgpack-cxx)
endif()
find_package(msgpack-cxx REQUIRED)

# libzmq cppzmq
find_package(cppzmq QUIET)
if(NOT cppzmq_FOUND)
    option(BUILD_SHARED "Whether or not to build the zmq shared object" OFF)
    option(BUILD_STATIC "Whether or not to build the zmq static archive" ON)
    option(ENABLE_DRAFTS "Build and install zmq draft classes and methods" ON)
    option(
        ENABLE_NO_EXPORT
        "Build with empty ZMQ_EXPORT macro, bypassing platform-based automated detection"
        ON
    )
    set(ZMQ_BUILD_TESTS OFF CACHE BOOL "Build the tests for ZeroMQ")
    FetchContent_Declare(
        libzmq
        GIT_REPOSITORY https://github.com/zeromq/libzmq
        GIT_TAG v4.3.5
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    option(CPPZMQ_BUILD_TESTS "Whether or not to build the cppzmq tests" OFF)
    FetchContent_Declare(
        cppzmq
        GIT_REPOSITORY https://github.com/zeromq/cppzmq
        GIT_TAG v4.10.0
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(libzmq cppzmq)
endif()
find_package(cppzmq REQUIRED)
