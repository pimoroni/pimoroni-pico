add_executable(
  ioe_adc_bg
  ioe_adc_bg.cpp
)

# Pull in pico libraries that we need
target_link_libraries(ioe_adc_bg pico_stdlib pico_explorer ms51xb9ae)

# disable usb output, enable uart output
pico_enable_stdio_usb(ioe_adc_bg 0)
pico_enable_stdio_uart(ioe_adc_bg 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(ioe_adc_bg)
