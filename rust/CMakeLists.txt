set(TRAEGER_RUST_COMPILER ${RUSTC} --edition=2021)

add_custom_target(
    traeger_rust
    ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libtraeger.rlib
)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libtraeger.rlib
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
        ${TRAEGER_RUST_COMPILER} --crate-type lib --crate-name traeger
        src/lib.rs -L ${TRAEGER_LIBRARY_PATH} --out-dir
        ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS traeger::actor traeger::format traeger::socket traeger::value
)

function(add_rust_target OUTPUT TYPE SOURCE)
    add_custom_target(
        rust-${OUTPUT}
        ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND
            ${TRAEGER_RUST_COMPILER} --crate-type ${TYPE} ${SOURCE}.rs --extern
            traeger=${CMAKE_CURRENT_BINARY_DIR}/../libtraeger.rlib -L
            ${TRAEGER_LIBRARY_PATH} -o ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
        DEPENDS traeger_rust ${SOURCE}.rs
    )
endfunction()

function(add_rust_example EXAMPLE)
    add_rust_target(${EXAMPLE} bin ${EXAMPLE})
endfunction()

add_subdirectory(examples)
