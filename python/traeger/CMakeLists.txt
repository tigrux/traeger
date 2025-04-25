set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

nanobind_add_module(
    traeger
    traeger.cpp
    traeger.hpp
    actor.cpp
    format.cpp
    socket.cpp
    value.cpp
)
target_link_libraries(
    traeger
    PRIVATE traeger::actor traeger::format traeger::socket traeger::value
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(traeger PRIVATE -Wno-maybe-uninitialized)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
endif()
