set(OUTPUT_NAME servo2040_multiple_servos)
add_executable(${OUTPUT_NAME} servo2040_multiple_servos.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        servo2040
        )

# enable usb output, disable uart output (so it doesn't confuse any connected servos)
pico_enable_stdio_usb(${OUTPUT_NAME} 1)
pico_enable_stdio_uart(${OUTPUT_NAME} 0)

pico_add_extra_outputs(${OUTPUT_NAME})
