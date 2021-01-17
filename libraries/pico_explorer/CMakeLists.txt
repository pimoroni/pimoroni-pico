add_library(pico_explorer INTERFACE)

target_sources(pico_explorer INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_explorer.cpp
)

target_include_directories(pico_explorer INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_explorer INTERFACE pico_stdlib hardware_pwm hardware_adc st7789 pico_graphics)