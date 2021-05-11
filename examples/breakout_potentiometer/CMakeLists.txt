set(OUTPUT_NAME potentiometer_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib breakout_potentiometer)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
