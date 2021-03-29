add_executable(
  wireless_time
  demo.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(wireless_time 1)
pico_enable_stdio_uart(wireless_time 0)

# Pull in pico libraries that we need
target_link_libraries(wireless_time pico_stdlib pico_wireless)

# create map/bin/hex file etc.
pico_add_extra_outputs(wireless_time)
