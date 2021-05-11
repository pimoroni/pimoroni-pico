add_executable(
  ioe_adc_min_case
  ioe_adc_min_case.cpp
)

# Pull in pico libraries that we need
target_link_libraries(ioe_adc_min_case pico_stdlib ioexpander)

# disable usb output, enable uart output
pico_enable_stdio_usb(ioe_adc_min_case 1)
pico_enable_stdio_uart(ioe_adc_min_case 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ioe_adc_min_case)
