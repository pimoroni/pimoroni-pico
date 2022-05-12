set(OUTPUT_NAME motorshim_movements)
add_executable(${OUTPUT_NAME} motorshim_movements.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        pico_motor_shim
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
