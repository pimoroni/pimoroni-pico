set(OUTPUT_NAME plasma2040_rainbow)
add_executable(${OUTPUT_NAME} plasma2040_rainbow.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        plasma2040
        rgbled
        button
        analog
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
