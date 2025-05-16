if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.24)
    project(traeger-examples CXX)
    set(version 0.1.0)
    set(CMAKE_CXX_STANDARD 17)
    find_package(traeger REQUIRED)
endif()

# Examples in the README.md

add_executable(example-value-semantics example-value-semantics.cpp)
target_link_libraries(example-value-semantics traeger::value)

add_executable(example-format-serialization example-format-serialization.cpp)
target_link_libraries(example-format-serialization traeger::format)

add_executable(
    example-actor-local
    example-actor-definition.cpp
    example-actor-messaging.cpp
    example-actor-local.cpp
)
target_link_libraries(example-actor-local traeger::actor)

add_executable(
    example-socket-replier
    example-actor-definition.cpp
    example-socket-replier.cpp
)
target_link_libraries(example-socket-replier traeger::socket)

add_executable(
    example-socket-requester
    example-actor-messaging.cpp
    example-socket-requester.cpp
)
target_link_libraries(example-socket-requester traeger::socket)

add_executable(example-socket-publisher example-socket-publisher.cpp)
target_link_libraries(example-socket-publisher traeger::socket)

add_executable(example-socket-subscriber example-socket-subscriber.cpp)
target_link_libraries(example-socket-subscriber traeger::socket)

add_executable(
    example-module-actor
    example-module-actor.cpp
    example-actor-messaging.cpp
)
target_link_libraries(example-module-actor traeger::module)

add_library(
    example-module
    MODULE
    example-module.cpp
    example-actor-definition.cpp
)
target_link_libraries(example-module traeger::actor)
set_target_properties(example-module PROPERTIES PREFIX "")
set_target_properties(example-module PROPERTIES SUFFIX ".module")
