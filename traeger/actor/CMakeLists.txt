set(TRAEGER_ACTOR_HEADERS
    actor.h
    Actor.hpp
    Mailbox.hpp
    Promise.hpp
    Queue.hpp
    Result.hpp
    Scheduler.hpp
    StatelessActor.hpp
)

target_link_libraries(traeger_actor PUBLIC traeger::value traeger::format)

find_package(Immer REQUIRED)
find_package(Threads REQUIRED)
if(
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
)
    target_compile_options(traeger_actor PRIVATE -Wno-array-bounds)
endif()
target_link_libraries(traeger_actor PRIVATE immer Threads::Threads)

target_sources(traeger_actor PRIVATE ${TRAEGER_ACTOR_HEADERS})
target_sources(
    traeger_actor
    PRIVATE
        Mailbox.cpp
        Promise.cpp
        Result.cpp
        Queue.cpp
        Scheduler.cpp
        StatelessActor.cpp
        traeger_actor.cpp
        traeger_actor.hpp
)

install(FILES ${TRAEGER_ACTOR_HEADERS} DESTINATION include/traeger/actor)
install(TARGETS traeger_actor DESTINATION lib EXPORT traeger-targets)
