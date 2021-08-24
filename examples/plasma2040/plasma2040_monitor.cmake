set(OUTPUT_NAME plasma2040_monitor)
add_executable(${OUTPUT_NAME} plasma2040_monitor.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        plasma2040
        bme68x
        hardware_i2c
        pimoroni_i2c
        rgbled
        button
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
