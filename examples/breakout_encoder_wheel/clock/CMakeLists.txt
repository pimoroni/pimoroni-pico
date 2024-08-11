set(OUTPUT_NAME encoderwheel_clock)
add_executable(${OUTPUT_NAME} clock.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME}
  pico_stdlib
  hardware_rtc
  breakout_encoder_wheel
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
