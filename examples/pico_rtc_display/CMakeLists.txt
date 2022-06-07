set(OUTPUT_NAME rtc_display)

add_executable(
  ${OUTPUT_NAME}
  pico_rtc_display.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_explorer pico_display breakout_rtc pico_graphics st7789 button rgbled)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
