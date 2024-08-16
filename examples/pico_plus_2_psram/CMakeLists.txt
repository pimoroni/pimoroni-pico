set(OUTPUT_NAME pico_plus_2_psram)

add_executable(
  ${OUTPUT_NAME}
  pico_plus_2_psram.cpp
  rp2_psram.cpp
)
   
add_subdirectory(lwmem)
# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib lwmem)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
