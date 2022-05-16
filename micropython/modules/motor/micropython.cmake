set(MOD_NAME motor)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/motor/motor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/motor/motor_cluster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/motor/motor_state.cpp
)
# pico_generate_pio_header(usermod_${MOD_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.pio)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/motor/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_PWM_ENABLED=1
    MODULE_MOTOR_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})