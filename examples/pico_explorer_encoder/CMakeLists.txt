add_executable(
  pico_explorer_encoder
  pico_explorer_encoder.cpp
)

pico_generate_pio_header(pico_explorer_encoder ${CMAKE_CURRENT_LIST_DIR}/quadrature_out.pio)

# Pull in pico libraries that we need
target_link_libraries(pico_explorer_encoder pico_stdlib pico_explorer encoder button motor picographics_st7789)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_explorer_encoder)