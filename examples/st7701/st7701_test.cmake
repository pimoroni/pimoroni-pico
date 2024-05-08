set(OUTPUT_NAME st7701_test)
add_executable(${OUTPUT_NAME} st7701_test.cpp)

target_link_libraries(${OUTPUT_NAME}
        hardware_spi
        pico_graphics
        st7701
        button
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
