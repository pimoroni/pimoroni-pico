set(MOD_NAME pwm)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.cpp
)
pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.pio)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_PWM_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})