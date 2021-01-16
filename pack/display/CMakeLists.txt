add_executable(
  display
  demo.cpp
  image_data.cpp
  pico_display.cpp
)

# Pull in pico libraries that we need
target_link_libraries(display pico_stdlib hardware_spi hardware_pwm hardware_dma st7789 pico_graphics)

# create map/bin/hex file etc.
pico_add_extra_outputs(display)