file(GLOB TRAEGER_GO_FILES ${CMAKE_SOURCE_DIR}/go/*.go)

function(add_go_example EXAMPLE)
    add_custom_target(
        go-${EXAMPLE}
        ALL
        DEPENDS ${CMAKE_BINARY_DIR}/go/examples/${EXAMPLE}/${EXAMPLE}
    )
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/go/examples/${EXAMPLE}/${EXAMPLE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND
            CC=${CMAKE_C_COMPILER} GOEXPERIMENT=rangefunc
            CGO_CFLAGS=-I${CMAKE_SOURCE_DIR}
            CGO_LDFLAGS=-L${TRAEGER_LIBRARY_PATH}
            LD_RUN_PATH=${TRAEGER_LIBRARY_PATH} ${GO} build -o
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${TRAEGER_GO_FILES}
        DEPENDS traeger::value traeger::format traeger::actor traeger::socket
        DEPENDS ${ARGN}
    )
endfunction()

add_subdirectory(example-value-semantics)
add_subdirectory(example-format-serialization)
add_subdirectory(example-actor-local)
add_subdirectory(example-socket-replier)
add_subdirectory(example-socket-requester)
add_subdirectory(example-socket-publisher)
add_subdirectory(example-socket-subscriber)
