set(TRAEGER_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

add_library(traeger_value SHARED)
add_library(traeger::value ALIAS traeger_value)
add_subdirectory(value)

add_library(traeger_format SHARED)
add_library(traeger::format ALIAS traeger_format)
add_subdirectory(format)

add_library(traeger_actor SHARED)
add_library(traeger::actor ALIAS traeger_actor)
add_subdirectory(actor)

add_library(traeger_socket SHARED)
add_library(traeger::socket ALIAS traeger_socket)
add_subdirectory(socket)

include(CTest)
if(BUILD_TESTING)
    add_executable(test-value)
    add_executable(test-format)
    add_executable(test-socket)
    add_executable(test-actor)

    add_subdirectory(tests)
    catch_discover_tests(test-value)
    catch_discover_tests(test-format)
    catch_discover_tests(test-socket)
    catch_discover_tests(test-actor)

    add_custom_target(
        test
        "ctest"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS test-value test-format test-socket test-actor
    )
endif()

set_target_properties(
    traeger_value
    traeger_format
    traeger_actor
    traeger_socket
    PROPERTIES VERSION ${version} SOVERSION 0
)
