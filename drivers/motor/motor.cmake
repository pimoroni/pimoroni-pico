set(LIB_NAME motor)
add_library(${LIB_NAME} INTERFACE)

target_sources(motor INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/motor.cpp
)

target_include_directories(motor INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(motor INTERFACE pico_stdlib hardware_pwm)