set(OUTPUT_NAME encoderwheel_interrupt)
add_executable(${OUTPUT_NAME} interrupt.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME}
  pico_stdlib
  breakout_encoder_wheel
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
