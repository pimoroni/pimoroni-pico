set(OUTPUT_NAME encoder_value_dial)
add_executable(${OUTPUT_NAME} encoder_value_dial.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        encoder
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
