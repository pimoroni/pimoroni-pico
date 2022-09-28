cmake_minimum_required(VERSION 3.13)
set(OUTPUT_NAME usb_comp_dev)
add_executable($OUTPUT_NAME)

target_sources($OUTPUT_NAME PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories($OUTPUT_NAME PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries($OUTPUT_NAME PUBLIC 
                        pico_stdlib 
                        tinyusb_device 
                        tinyusb_board
                        tufty2040
                        hardware_spi
                        pico_graphics
                        st7789
                        button
                        )

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions($OUTPUT_NAME PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs($OUTPUT_NAME)

# add url via pico_set_program_url
example_auto_set_url($OUTPUT_NAME)
