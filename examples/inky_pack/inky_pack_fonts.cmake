set(OUTPUT_NAME inky_pack_fonts)
add_executable(${OUTPUT_NAME} inky_pack_fonts.cpp)

target_link_libraries(${OUTPUT_NAME}
        badger2040
        pico_stdlib 
        hardware_spi 
        hardware_pwm 
        hardware_dma  
        button 
        uc8151 
        pico_graphics
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
