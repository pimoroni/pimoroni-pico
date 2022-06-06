set(OUTPUT_NAME inky_frame_sleepy_head)

add_executable(
  ${OUTPUT_NAME}
  inky_frame_sleepy_head.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib inky_frame)

pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
