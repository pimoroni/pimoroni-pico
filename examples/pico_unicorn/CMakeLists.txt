add_executable(
  unicorn
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(unicorn pico_stdlib hardware_pio hardware_dma pico_graphics pico_unicorn)

# create map/bin/hex file etc.
pico_add_extra_outputs(unicorn)
