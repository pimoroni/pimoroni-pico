if(NOT TARGET analog)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/analog/analog.cmake)
endif()

add_library(automation INTERFACE)

target_sources(automation INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/automation.cpp
)

target_include_directories(automation INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(automation INTERFACE pico_stdlib hardware_pwm hardware_i2c pimoroni_i2c analog)
