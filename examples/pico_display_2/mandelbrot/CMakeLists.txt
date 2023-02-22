set(OUTPUT_NAME display_2_mandelbrot)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_multicore hardware_spi hardware_pwm hardware_dma rgbled button pico_display_2 st7789 pico_graphics)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
