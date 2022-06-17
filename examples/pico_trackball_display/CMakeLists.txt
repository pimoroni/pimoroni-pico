set(OUTPUT_NAME trackball_display)

add_executable(
  ${OUTPUT_NAME}
  pico_trackball_display.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_explorer pico_display breakout_trackball pico_graphics st7789)

pico_enable_stdio_uart(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})

#example_auto_set_url(${OUTPUT_NAME})
