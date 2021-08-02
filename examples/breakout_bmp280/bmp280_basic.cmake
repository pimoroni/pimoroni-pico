set(OUTPUT_NAME bmp280_basic_demo)

add_executable(
  ${OUTPUT_NAME}
  bmp280_basic.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib bmp280)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
