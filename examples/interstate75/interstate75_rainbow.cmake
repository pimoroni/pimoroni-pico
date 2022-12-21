set(OUTPUT_NAME interstate75_rainbow)
add_executable(${OUTPUT_NAME} interstate75_rainbow.cpp)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})

target_link_libraries(${OUTPUT_NAME}
    pico_stdlib
    pico_multicore
    hardware_vreg
    hardware_pwm
    pico_graphics
    hub75
    interstate75
)
