set(OUTPUT_NAME camera_pack_sdcard_test)

add_executable(
  ${OUTPUT_NAME}
  camera_pack_sdcard_test.cpp
)

pico_enable_stdio_uart(${OUTPUT_NAME} 0)
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

add_definitions(-DSDCARD_PIN_SPI0_CS=26 -DSDCARD_PIN_SPI0_MISO=20)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_camera fatfs sdcard hardware_pio)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})