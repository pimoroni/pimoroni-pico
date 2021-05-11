set(OUTPUT_NAME bme688_forced)

add_executable(
  ${OUTPUT_NAME}
  ${OUTPUT_NAME}.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib bme68x)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
