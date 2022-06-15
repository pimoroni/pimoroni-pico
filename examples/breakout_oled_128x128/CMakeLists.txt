set(OUTPUT_NAME oled_128x128_demo)

add_executable(
  ${OUTPUT_NAME}
  oled_128x128_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pico_graphics sh1107)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})