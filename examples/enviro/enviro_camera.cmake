set(OUTPUT_NAME enviro_camera)
add_executable(${OUTPUT_NAME} enviro_camera.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME}
    pico_stdlib
    enviro
    )

add_compile_definitions(PICO_STDIO_USB_TASK_INTERVAL_US=100)
add_compile_definitions(PICO_STDIO_USB_LOW_PRIORITY_IRQ=26)

# enable usb output and disable uart (as those pins are used)
pico_enable_stdio_usb(${OUTPUT_NAME} 1)
pico_enable_stdio_uart(${OUTPUT_NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
