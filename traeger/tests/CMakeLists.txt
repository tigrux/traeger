# catch2
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.8.1.tar.gz
        URL_HASH
            SHA256=18b3f70ac80fccc340d8c6ff0f339b2ae64944782f8d2fca2bd705cf47cadb79
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(Catch2)
endif()
find_package(Catch2 3 REQUIRED)

include(Catch)
target_sources(
    test-value
    PRIVATE
        test-list-append.cpp
        test-list-empty.cpp
        test-list-find.cpp
        test-list-iterator.cpp
        test-list-resize.cpp
        test-list-set.cpp
        test-list-size.cpp
        test-list-unpack.cpp
        test-map-empty.cpp
        test-map-find.cpp
        test-map-get.cpp
        test-map-iterator.cpp
        test-map-set.cpp
        test-map-size.cpp
        test-value-equals.cpp
        test-value-get.cpp
        test-value-set.cpp
        test-value-type_name.cpp
        test-value-type.cpp
)
target_link_libraries(test-value PRIVATE Catch2::Catch2WithMain traeger::value)

target_sources(
    test-format
    PRIVATE
        test-format-by_name.cpp
        test-format-decode.cpp
        test-format-encode.cpp
        test-format-name.cpp
)
target_link_libraries(
    test-format
    PRIVATE Catch2::Catch2WithMain traeger::format
)

target_sources(
    test-actor
    PRIVATE
        test-actor-define.cpp
        test-mailbox-send.cpp
        test-promise-fail.cpp
        test-promise-promise.cpp
        test-promise-result.cpp
        test-promise-scheduler.cpp
        test-promise-then.cpp
        test-queue-close.cpp
        test-queue-count.cpp
        test-queue-pop.cpp
        test-queue-push.cpp
        test-result-equals.cpp
        test-result-error.cpp
        test-result-set.cpp
        test-result-type_name.cpp
        test-result-type.cpp
        test-result-value.cpp
        test-scheduler-count.cpp
        test-scheduler-schedule_delayed.cpp
        test-scheduler-schedule.cpp
        test-stateless_actor-define.cpp
)
target_link_libraries(test-actor PRIVATE Catch2::Catch2WithMain traeger::actor)

target_sources(
    test-socket
    PRIVATE
        test-context-publisher.cpp
        test-context-replier.cpp
        test-context-requester.cpp
        test-context-subscriber.cpp
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test-socket PRIVATE -Wno-maybe-uninitialized)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20)
        target_compile_options(
            test-socket
            PRIVATE -Wno-deprecated-literal-operator
        )
    endif()
endif()

target_link_libraries(
    test-socket
    PRIVATE Catch2::Catch2WithMain traeger::socket
)
find_package(cppzmq REQUIRED)
if(TARGET libzmq-static)
    target_link_libraries(test-socket PRIVATE cppzmq-static)
else()
    target_link_libraries(test-socket PRIVATE cppzmq)
endif()
