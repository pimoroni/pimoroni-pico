set(DRIVER_NAME servo)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/servo.cpp
  ${CMAKE_CURRENT_LIST_DIR}/calibration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/servo_state.cpp
)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${DRIVER_NAME} INTERFACE
    pico_stdlib
    hardware_pwm
    pwm
    )