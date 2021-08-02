set(OUTPUT_NAME bme280_basic_demo)

add_executable(
  ${OUTPUT_NAME}
  bme280_basic.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib bme280)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
