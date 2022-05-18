add_library(usermod_hub75 INTERFACE)

target_sources(usermod_hub75 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hub75.c
    ${CMAKE_CURRENT_LIST_DIR}/hub75.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/hub75/hub75.cpp
)

target_include_directories(usermod_hub75 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/hub75/
)

target_compile_definitions(usermod_hub75 INTERFACE
    MODULE_HUB75_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_hub75)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/hub75.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)

pico_generate_pio_header(usermod_hub75 ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/hub75/hub75.pio)