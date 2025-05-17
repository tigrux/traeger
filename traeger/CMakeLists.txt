set(TRAEGER_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

foreach(
    LIBRARY
    value
    format
    actor
    socket
    module
)
    add_library(traeger_${LIBRARY} SHARED)
    list(APPEND TRAEGER_LIBRARIES traeger_${LIBRARY})
    add_library(traeger::${LIBRARY} ALIAS traeger_${LIBRARY})
    add_subdirectory(${LIBRARY})
endforeach()

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
    ${TRAEGER_LIBRARIES}
    PROPERTIES VERSION ${version} SOVERSION 0
)
