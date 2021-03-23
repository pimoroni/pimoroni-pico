add_library(usermod_pico_rgb_keypad INTERFACE)

target_sources(usermod_pico_rgb_keypad INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pico_rgb_keypad.c
    ${CMAKE_CURRENT_LIST_DIR}/pico_rgb_keypad.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_rgb_keypad/pico_rgb_keypad.cpp
)

target_include_directories(usermod_pico_rgb_keypad INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_pico_rgb_keypad INTERFACE
    MODULE_PICOKEYPAD_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_pico_rgb_keypad)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/pico_rgb_keypad.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)
