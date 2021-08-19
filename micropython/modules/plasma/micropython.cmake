set(MOD_NAME plasma)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/plasma/apa102.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/plasma/ws2812.cpp
)
pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/plasma/apa102.pio)
pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/plasma/ws2812.pio)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/plasma/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_PLASMA_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})