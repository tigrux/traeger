set(TRAEGER_SOCKET_HEADERS
    Context.hpp
    Publisher.hpp
    Replier.hpp
    Requester.hpp
    socket.h
    Socket.hpp
    Subscriber.hpp
)

target_compile_definitions(traeger_socket PRIVATE -DZMQ_BUILD_DRAFT_API)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(traeger_socket PRIVATE -Wno-maybe-uninitialized)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20)
        target_compile_options(
            traeger_socket
            PRIVATE -Wno-deprecated-literal-operator
        )
    endif()
endif()

if(TARGET libzmq-static)
    target_link_libraries(traeger_socket PRIVATE cppzmq-static)
else()
    target_link_libraries(traeger_socket PRIVATE cppzmq)
endif()

target_link_libraries(
    traeger_socket
    PUBLIC traeger::value traeger::format traeger::actor
)

target_sources(traeger_socket PRIVATE ${TRAEGER_SOCKET_HEADERS})
target_sources(
    traeger_socket
    PRIVATE
        Context.cpp
        Publisher.cpp
        Replier.cpp
        Requester.cpp
        Socket.cpp
        Subscriber.cpp
        traeger_socket.cpp
        traeger_socket.hpp
)

install(FILES ${TRAEGER_SOCKET_HEADERS} DESTINATION include/traeger/socket)
install(TARGETS traeger_socket DESTINATION lib EXPORT traeger-targets)
