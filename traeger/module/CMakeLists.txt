set(TRAEGER_MODULE_HEADERS Module.hpp module.h)

target_link_libraries(traeger_module PUBLIC traeger::actor)
if(NOT WIN32)
    target_link_libraries(traeger_module PUBLIC dl)
endif()

target_sources(traeger_module PRIVATE ${TRAEGER_MODULE_HEADERS})
target_sources(
    traeger_module
    PRIVATE traeger_module.cpp traeger_module.hpp Module.cpp
)

install(FILES ${TRAEGER_MODULE_HEADERS} DESTINATION include/traeger/module)
install(TARGETS traeger_module DESTINATION lib EXPORT traeger-targets)
