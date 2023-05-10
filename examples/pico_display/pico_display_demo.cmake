add_executable(
  pico_display_demo
  pico_display_demo.cpp
  image_data.cpp
)

# Pull in pico libraries that we need
target_link_libraries(pico_display_demo pico_stdlib hardware_spi hardware_pwm hardware_dma rgbled pico_display pico_graphics st7789)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_display_demo)