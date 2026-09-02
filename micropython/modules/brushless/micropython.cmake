set(MOD_NAME brushless)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../pin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_alloc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/brushless/brushless.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/brushless/brushless_cluster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/brushless/brushless_state.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/brushless/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_PWM_ENABLED=1
    MODULE_BRUSHLESS_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})
