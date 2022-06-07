add_executable(
  tof_display
  pico_tof_display.cpp
)

# Pull in pico libraries that we need
target_link_libraries(tof_display pico_stdlib pico_explorer pico_display vl53l1x pico_graphics st7789 button)

pico_enable_stdio_uart(tof_display 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(tof_display)

#example_auto_set_url(tof_display)
