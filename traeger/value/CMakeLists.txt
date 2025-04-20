set(TRAEGER_VALUE_HEADERS
    List.hpp
    Map.hpp
    types.h
    Types.hpp
    value.h
    Value.hpp
    Variant.hpp
)

if(
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
)
    target_compile_options(traeger_value PRIVATE -Wno-array-bounds)
endif()
target_link_libraries(traeger_value PRIVATE immer)

target_sources(traeger_value PRIVATE ${TRAEGER_VALUE_HEADERS})
target_sources(
    traeger_value
    PRIVATE
        List_impl.hpp
        List.cpp
        Map_impl.hpp
        Map.cpp
        traeger_value.cpp
        traeger_value.hpp
        Value_impl.hpp
        Value.cpp
        Variant.cpp
)

install(FILES ${TRAEGER_VALUE_HEADERS} DESTINATION include/traeger/value)
install(TARGETS traeger_value DESTINATION lib EXPORT traeger-targets)
