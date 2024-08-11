if(NOT TARGET pwm_cluster)
    include(${CMAKE_CURRENT_LIST_DIR}/../pwm/pwm_cluster.cmake)
endif()

set(DRIVER_NAME motor_cluster)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/motor_cluster.cpp
  ${CMAKE_CURRENT_LIST_DIR}/motor_state.cpp
)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${DRIVER_NAME} INTERFACE
    pico_stdlib
    pwm_cluster
    )