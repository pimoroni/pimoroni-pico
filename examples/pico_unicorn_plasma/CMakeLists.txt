add_executable(
  unicornplasma
  demo.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(unicornplasma 1)
pico_enable_stdio_uart(unicornplasma 0)

# Pull in pico libraries that we need
target_link_libraries(unicornplasma pico_stdlib hardware_pio hardware_dma pico_unicorn)

# create map/bin/hex file etc.
pico_add_extra_outputs(unicornplasma)
