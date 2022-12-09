add_executable(
  ili9341_demo
  ili9341_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(ili9341_demo pico_stdlib hardware_spi hardware_pwm hardware_dma rgbled pico_display pico_graphics ili9341 xpt2046)

# create map/bin/hex file etc.
pico_add_extra_outputs(ili9341_demo)


add_executable(
  ili9341_async_region
  ili9341_async_region.cpp
	calibrate_screen.cpp
)

# Pull in pico libraries that we need
target_link_libraries(ili9341_async_region pico_stdlib hardware_spi hardware_pwm hardware_dma rgbled pico_display pico_graphics ili9341 xpt2046)

# create map/bin/hex file etc.
pico_add_extra_outputs(ili9341_async_region)