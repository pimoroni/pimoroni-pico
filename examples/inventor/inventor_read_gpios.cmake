set(OUTPUT_NAME inventor2040w_read_gpios)
add_executable(${OUTPUT_NAME} inventor2040w_read_gpios.cpp)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib inventor)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# disable the uart to avoid issues with reading GP0 and GP1
pico_enable_stdio_uart(${OUTPUT_NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
