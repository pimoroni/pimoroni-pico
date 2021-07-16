set(OUTPUT_NAME scd41_demo)

add_executable(
  ${OUTPUT_NAME}
  scd41_demo.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)
pico_enable_stdio_uart(${OUTPUT_NAME} 1)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib scd4x)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
