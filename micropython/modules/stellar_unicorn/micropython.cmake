set(MOD_NAME stellar_unicorn)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/stellar_unicorn/stellar_unicorn.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_synth/pico_synth.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_rgb888.cpp
)
pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/stellar_unicorn/stellar_unicorn.pio)
pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/stellar_unicorn/audio_i2s.pio)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_STELLAR_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})