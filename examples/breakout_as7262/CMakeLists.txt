add_executable(
  breakout_as7262
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(breakout_as7262 pico_stdlib hardware_spi as7262)

# create map/bin/hex file etc.
pico_add_extra_outputs(breakout_as7262)