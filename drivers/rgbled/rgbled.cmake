add_library(rgbled INTERFACE)

target_sources(rgbled INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/rgbled.cpp
)

target_include_directories(rgbled INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(rgbled INTERFACE pico_stdlib hardware_pwm)