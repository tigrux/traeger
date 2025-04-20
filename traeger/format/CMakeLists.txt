set(TRAEGER_FORMAT_HEADERS Format.hpp format.h)

target_link_libraries(traeger_format PUBLIC traeger::value)

target_link_libraries(traeger_format PRIVATE immer)

target_sources(traeger_format PRIVATE ${TRAEGER_FORMAT_HEADERS})
target_sources(
    traeger_format
    PRIVATE Format.cpp traeger_format.hpp traeger_format.cpp
)

if(nlohmann_json_FOUND)
    target_link_libraries(traeger_format PRIVATE nlohmann_json)
    target_compile_definitions(traeger_format PUBLIC TRAEGER_FORMAT_JSON=1)
    target_sources(traeger_format PRIVATE Format_json.cpp)
endif()

if(yaml-cpp_FOUND)
    target_link_libraries(traeger_format PRIVATE yaml-cpp::yaml-cpp)
    target_compile_definitions(traeger_format PUBLIC TRAEGER_FORMAT_YAML=1)
    target_sources(traeger_format PRIVATE Format_yaml.cpp)
endif()

if(msgpack-cxx_FOUND)
    target_link_libraries(traeger_format PRIVATE msgpack-cxx)
    target_compile_definitions(traeger_format PUBLIC TRAEGER_FORMAT_MSGPACK=1)
    target_sources(traeger_format PRIVATE Format_msgpack.cpp)
endif()

install(FILES ${TRAEGER_FORMAT_HEADERS} DESTINATION include/traeger/format)
install(TARGETS traeger_format DESTINATION lib EXPORT traeger-targets)
