set(OUTPUT_NAME ads1015_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_i2c breakout_ads1015)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
