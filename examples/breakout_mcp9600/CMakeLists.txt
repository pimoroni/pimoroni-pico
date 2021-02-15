set(OUTPUT_NAME mcp9600_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_i2c breakout_mcp9600)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
