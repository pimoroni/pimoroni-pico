set(OUTPUT_NAME motor2040_single_motor)
add_executable(${OUTPUT_NAME} motor2040_single_motor.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        motor2040
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
