set(OUTPUT_NAME badger2040w_drawing)
add_executable(${OUTPUT_NAME} badger2040w_drawing.cpp)

target_link_libraries(${OUTPUT_NAME}
        badger2040w
        hardware_spi
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
