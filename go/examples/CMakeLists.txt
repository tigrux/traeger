file(GLOB TRAEGER_GO_FILES ${CMAKE_SOURCE_DIR}/go/*.go)

function(add_go_target OUTPUT TYPE)
    add_custom_target(
        go-${OUTPUT}
        ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND
            CC=${CMAKE_C_COMPILER} GOEXPERIMENT=rangefunc
            CGO_CFLAGS=-I${CMAKE_SOURCE_DIR}
            CGO_LDFLAGS=-L${TRAEGER_LIBRARY_PATH}
            LD_RUN_PATH=${TRAEGER_LIBRARY_PATH} ${GO} build -o
            ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT} -buildmode=${TYPE}
        DEPENDS ${TRAEGER_GO_FILES} ${TRAEGER_LIBRARIES} ${ARGN}
    )
endfunction()

function(add_go_example EXAMPLE)
    add_go_target(${EXAMPLE} exe)
endfunction()

function(add_go_module MODULE)
    add_go_target(${MODULE}.module c-shared)
endfunction()

add_subdirectory(example-value-semantics)
add_subdirectory(example-format-serialization)
add_subdirectory(example-actor-local)
add_subdirectory(example-socket-replier)
add_subdirectory(example-socket-requester)
add_subdirectory(example-socket-publisher)
add_subdirectory(example-socket-subscriber)
add_subdirectory(example-module-actor)
add_subdirectory(example-module)
