add_library(usermod_picovector INTERFACE)

target_sources(usermod_picovector INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/picovector.c
    ${CMAKE_CURRENT_LIST_DIR}/picovector.cpp
)

target_include_directories(usermod_picovector INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_picovector INTERFACE
    MODULE_PICOVECTOR_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_picovector)
