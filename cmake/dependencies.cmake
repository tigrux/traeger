# immer
find_package(Immer QUIET)
if(NOT Immer_FOUND)
    option(immer_BUILD_TESTS "Build immer tests" OFF)
    option(immer_BUILD_EXAMPLES "Build immer examples" OFF)
    option(immer_BUILD_DOCS "Build immer docs" ON)
    option(immer_BUILD_EXTRAS "Build immer extras" ON)
    FetchContent_Declare(
        immer
        URL https://github.com/arximboldi/immer/archive/refs/tags/v0.8.1.tar.gz
        URL_HASH
            SHA256=de8411c84830864604bb685dc8f2e3c0dbdc40b95b2f6726092f7dcc85e75209
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
        URL https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz
        URL_HASH
            SHA256=4b92eb0c06d10683f7447ce9406cb97cd4b453be18d7279320f7b2f025c10187
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
        URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.tar.gz
        URL_HASH
            SHA256=fbe74bbdcee21d656715688706da3c8becfd946d92cd44705cc6098bb23b3a16
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
        URL
            https://github.com/msgpack/msgpack-c/archive/refs/tags/cpp-7.0.0.tar.gz
        URL_HASH
            SHA256=070881ebea9208cf7e731fd5a46a11404025b2f260ab9527e32dfcb7c689fbfc
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
        URL https://github.com/zeromq/libzmq/archive/refs/tags/v4.3.5.tar.gz
        URL_HASH
            SHA256=6c972d1e6a91a0ecd79c3236f04cf0126f2f4dfbbad407d72b4606a7ba93f9c6
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    option(CPPZMQ_BUILD_TESTS "Whether or not to build the cppzmq tests" OFF)
    FetchContent_Declare(
        cppzmq
        URL https://github.com/zeromq/cppzmq/archive/refs/tags/v4.10.0.tar.gz
        URL_HASH
            SHA256=c81c81bba8a7644c84932225f018b5088743a22999c6d82a2b5f5cd1e6942b74
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(libzmq cppzmq)
endif()
find_package(cppzmq REQUIRED)
