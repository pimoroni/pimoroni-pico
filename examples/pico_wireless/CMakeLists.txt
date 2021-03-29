add_executable(
  wireless_demo
  demo.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(wireless_demo 1)
pico_enable_stdio_uart(wireless_demo 0)

# Pull in pico libraries that we need
target_link_libraries(wireless_demo pico_stdlib pico_wireless)

# create map/bin/hex file etc.
pico_add_extra_outputs(wireless_demo)
