set(OUTPUT_NAME enviro_grow)
add_executable(${OUTPUT_NAME} enviro_grow.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME}
    pico_stdlib
    enviro
    )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
