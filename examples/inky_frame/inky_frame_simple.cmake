set(OUTPUT_NAME inky_frame_simple)

add_executable(
  ${OUTPUT_NAME}
  inky_frame_simple.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_pwm hardware_spi hardware_i2c hardware_rtc fatfs sdcard pico_graphics uc8159 )

pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
