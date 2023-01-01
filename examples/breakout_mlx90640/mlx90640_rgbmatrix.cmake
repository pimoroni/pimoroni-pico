set(OUTPUT_NAME mlx90460_rgbmatrix)

add_executable(
  ${OUTPUT_NAME}
  mlx90640_rgbmatrix.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib mlx90640 hub75 hardware_vreg)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
