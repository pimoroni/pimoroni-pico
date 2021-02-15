set(OUTPUT_NAME veml6075_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_i2c breakout_veml6075)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
