set(OUTPUT_NAME gfx_pack_demo)

add_executable(
  ${OUTPUT_NAME}
  gfx_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib gfx_pack pico_graphics)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})