add_library(pico_rgb_keypad INTERFACE)

target_sources(pico_rgb_keypad INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_rgb_keypad.cpp
)

target_include_directories(pico_rgb_keypad INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_rgb_keypad INTERFACE pico_stdlib hardware_i2c hardware_spi)