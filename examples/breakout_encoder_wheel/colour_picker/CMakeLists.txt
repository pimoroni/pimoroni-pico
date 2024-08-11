set(OUTPUT_NAME encoderwheel_colour_picker)
add_executable(${OUTPUT_NAME} colour_picker.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME}
  pico_stdlib
  breakout_encoder_wheel
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
