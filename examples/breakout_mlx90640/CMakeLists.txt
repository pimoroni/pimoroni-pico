set(OUTPUT_NAME mlx90460_demo)

add_executable(
  ${OUTPUT_NAME}
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib mlx90640 hub75_legacy hardware_vreg hardware_clocks)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
