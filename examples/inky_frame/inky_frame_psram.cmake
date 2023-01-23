set(OUTPUT_NAME inky_frame_psram)

add_executable(
  ${OUTPUT_NAME}
  inky_frame_psram.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib inky_frame hardware_pwm hardware_spi hardware_i2c hardware_rtc fatfs sdcard pico_graphics hardware_dma aps6404)

pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
