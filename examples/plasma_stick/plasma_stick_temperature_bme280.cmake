set(OUTPUT_NAME plasma_stick_temperature_bme280)
add_executable(${OUTPUT_NAME} plasma_stick_temperature_bme280.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        plasma_stick
        bme280
        pimoroni_i2c
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
