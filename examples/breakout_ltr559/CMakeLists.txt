add_executable(
  breakout_ltr559
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(breakout_ltr559 pico_stdlib hardware_spi ltr559)

# create map/bin/hex file etc.
pico_add_extra_outputs(breakout_ltr559)