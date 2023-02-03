set(MOD_NAME picographics)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/st7789/st7789.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/st7735/st7735.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/sh1107/sh1107.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/uc8151/uc8151.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/uc8159/uc8159.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/st7567/st7567.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/inky73/inky73.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/shiftregister/shiftregister.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/psram_display/psram_display.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_1bit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_1bitY.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_3bit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_p4.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_p8.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_rgb332.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_rgb565.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_rgb888.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/pico_graphics_pen_inky7.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/types.cpp
)

pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/st7789/st7789_parallel.pio)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    -DMODULE_${MOD_NAME_UPPER}_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers"
)
