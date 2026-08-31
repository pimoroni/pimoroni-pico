set(MOD_NAME servo_cluster)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../servo/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/../servo/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/pwm_cluster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/servo/servo_cluster.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../servo/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/pwm/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/servo/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_SERVO_CLUSTER_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})
