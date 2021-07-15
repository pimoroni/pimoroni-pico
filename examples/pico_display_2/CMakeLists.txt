set(OUTPUT_NAME display_2)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(  ${OUTPUT_NAME} pico_stdlib hardware_spi hardware_pwm hardware_dma pico_display_2)

# create map/bin/hex file etc.
pico_add_extra_outputs(  ${OUTPUT_NAME})