set(OUTPUT_NAME keycoder2040_demo)
add_executable(${OUTPUT_NAME} keycoder2040_demo.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        keycoder2040
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
