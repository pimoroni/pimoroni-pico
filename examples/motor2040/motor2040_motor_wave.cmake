set(OUTPUT_NAME motor2040_motor_wave)
add_executable(${OUTPUT_NAME} motor2040_motor_wave.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        motor2040
        button
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
