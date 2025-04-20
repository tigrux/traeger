set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

nanobind_add_module(
    traeger_python
    traeger_python_actor.cpp
    traeger_python_format.cpp
    traeger_python_socket.cpp
    traeger_python_value.cpp
    traeger_python.cpp
    traeger_python.hpp
)
target_link_libraries(
    traeger_python
    PRIVATE traeger::actor traeger::format traeger::socket traeger::value
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(traeger_python PRIVATE -Wno-maybe-uninitialized)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
endif()

set_target_properties(traeger_python PROPERTIES OUTPUT_NAME traeger)
