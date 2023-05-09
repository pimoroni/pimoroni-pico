set(OUTPUT_NAME dv_stick_test)

add_executable(
  ${OUTPUT_NAME}
  dv_stick_test.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib dv_display hardware_i2c pico_graphics)

pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
