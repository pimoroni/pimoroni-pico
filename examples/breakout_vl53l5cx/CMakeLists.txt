set(OUTPUT_NAME vl53l5cx_demo)

add_executable(
  ${OUTPUT_NAME}
  vl53l5cx_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_i2c vl53l5cx pimoroni_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
