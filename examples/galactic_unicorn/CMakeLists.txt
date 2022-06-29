add_executable(
  galactic_unicorn_demo
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(galactic_unicorn_demo pico_stdlib hardware_pio hardware_dma pico_graphics galactic_unicorn)

# create map/bin/hex file etc.
pico_add_extra_outputs(galactic_unicorn_demo)
