add_executable(
  st7789_round_demo
  round_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(st7789_round_demo pico_stdlib hardware_spi st7789 pico_graphics)

# create map/bin/hex file etc.
pico_add_extra_outputs(st7789_round_demo)

