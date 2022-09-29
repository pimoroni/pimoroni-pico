set(OUTPUT_NAME gfx_pack_demo)

add_executable(
  ${OUTPUT_NAME}
  gfx_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_spi hardware_pwm hardware_dma rgbled button pico_display_2 st7568 pico_graphics)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})