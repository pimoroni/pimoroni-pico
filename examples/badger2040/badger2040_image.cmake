set(OUTPUT_NAME badger2040_image)
add_executable(${OUTPUT_NAME} badger2040_image.cpp)

target_link_libraries(${OUTPUT_NAME}
        badger2040
        hardware_spi
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
