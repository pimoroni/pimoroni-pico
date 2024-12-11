set(OUTPUT_NAME pico_display28_demo)

add_executable(
  ${OUTPUT_NAME}
  pico_display_28_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_spi hardware_pwm hardware_dma rgbled button pico_display_28 st7789 pico_graphics)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})