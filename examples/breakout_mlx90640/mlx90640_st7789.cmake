set(OUTPUT_NAME mlx90640_st7789)

add_executable(
  ${OUTPUT_NAME}
  mlx90640_st7789.cpp
)

# Pull in pico libraries that we need
pico_enable_stdio_usb(${OUTPUT_NAME} 1)
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_stdio mlx90640 pico_graphics st7789)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
