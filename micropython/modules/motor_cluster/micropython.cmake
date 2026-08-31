set(MOD_NAME motor_cluster)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../motor/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/../motor/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/motor/motor_cluster.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../motor/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/motor/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_MOTOR_CLUSTER_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})
