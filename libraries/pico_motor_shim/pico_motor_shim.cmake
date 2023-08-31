if(NOT TARGET motor)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/motor/motor.cmake)
endif()

add_library(pico_motor_shim INTERFACE)

target_include_directories(pico_motor_shim INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_motor_shim INTERFACE pico_stdlib motor)