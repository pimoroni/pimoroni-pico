set(OUTPUT_NAME camera_pack_ascii)

add_executable(
  ${OUTPUT_NAME}
  camera_pack_ascii.cpp
)

pico_enable_stdio_uart(${OUTPUT_NAME} 0)
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_camera)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})