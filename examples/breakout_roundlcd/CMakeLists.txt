set(OUTPUT_NAME roundlcd_demo)

add_executable(
  ${OUTPUT_NAME}
  roundlcd_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_graphics st7789)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})