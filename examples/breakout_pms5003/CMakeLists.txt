set(OUTPUT_NAME pms5003_demo)

add_executable(
  ${OUTPUT_NAME}
  pms5003_demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib pms5003)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
