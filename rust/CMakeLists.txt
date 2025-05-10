add_custom_target(
    traeger_rust
    ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libtraeger.rlib
)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libtraeger.rlib
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
        ${RUSTC} --edition=2024 --crate-name traeger src/lib.rs --crate-type lib
        -L ${TRAEGER_LIBRARY_PATH} --out-dir ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS traeger::value traeger::format traeger::actor traeger::socket
)

function(add_rust_example EXAMPLE)
    add_custom_target(
        rust-${EXAMPLE}
        ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE}
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND
            ${RUSTC} --edition=2024 --crate-type bin ${EXAMPLE}.rs --extern
            traeger=${CMAKE_CURRENT_BINARY_DIR}/../libtraeger.rlib -L
            ${TRAEGER_LIBRARY_PATH} --out-dir ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS traeger_rust
    )
endfunction()

add_subdirectory(examples)
