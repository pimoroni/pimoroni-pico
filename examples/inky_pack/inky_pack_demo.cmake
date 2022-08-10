set(OUTPUT_NAME inky_pack_demo)
add_executable(${OUTPUT_NAME} inky_pack_demo.cpp)

target_link_libraries(${OUTPUT_NAME}
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
