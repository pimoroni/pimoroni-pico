set(OUTPUT_NAME max30105_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
  heartrate.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib hardware_i2c breakout_max30105)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
