set(OUTPUT_NAME inky_frame7_test)

add_executable(
  ${OUTPUT_NAME}
  inky_frame7_test.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib psram_display inky73 inky_frame hardware_pwm hardware_spi hardware_i2c fatfs sdcard pico_graphics)

pico_enable_stdio_usb(${OUTPUT_NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
