set(OUTPUT_NAME encoder_item_selector)
add_executable(${OUTPUT_NAME} encoder_item_selector.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        encoder
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
