set(OUTPUT_NAME encoder_explorer)

add_executable(
  ${OUTPUT_NAME}
  pico_enc_explorer.cpp
)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib breakout_encoder pico_explorer pico_graphics st7789)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
