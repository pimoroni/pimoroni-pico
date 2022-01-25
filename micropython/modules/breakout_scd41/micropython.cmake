set(MOD_NAME breakout_scd41)
set(DRIVER ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/scd4x)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${DRIVER}/src/scd4x_i2c.c
    ${DRIVER}/src/sensirion_common.c
    ${DRIVER}/src/sensirion_i2c.c
    ${DRIVER}/i2c_hal.cpp
    ${DRIVER}/scd4x.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${DRIVER}
    ${DRIVER}/src
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    -DMODULE_${MOD_NAME_UPPER}_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/breakout_scd41.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)