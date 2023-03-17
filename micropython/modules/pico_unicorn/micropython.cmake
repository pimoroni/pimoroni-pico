add_library(usermod_pico_unicorn INTERFACE)

target_sources(usermod_pico_unicorn INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pico_unicorn.c
    ${CMAKE_CURRENT_LIST_DIR}/pico_unicorn.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_unicorn/pico_unicorn.cpp
)

pico_generate_pio_header(usermod_pico_unicorn ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_unicorn/pico_unicorn.pio)

target_include_directories(usermod_pico_unicorn INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/
)

target_compile_definitions(usermod_pico_unicorn INTERFACE
    MODULE_PICOUNICORN_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_pico_unicorn)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/pico_unicorn.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)
