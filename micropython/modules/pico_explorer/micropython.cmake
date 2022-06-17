add_library(usermod_pico_explorer INTERFACE)

target_sources(usermod_pico_explorer INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pico_explorer.c
)

target_include_directories(usermod_pico_explorer INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_pico_explorer INTERFACE
    MODULE_PICOEXPLORER_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_pico_explorer)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/pico_explorer.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers"
)
