set(OUTPUT_NAME colourlcd160x80_demo)

add_executable(
  ${OUTPUT_NAME}
  colorlcd_160x80_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_graphics st7735 hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
