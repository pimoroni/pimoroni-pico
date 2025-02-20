set(OUTPUT_NAME inventor_position_on_velocity_control)
add_executable(${OUTPUT_NAME} inventor_position_on_velocity_control.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME}
    pico_stdlib
    inventor
    pid
    )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
