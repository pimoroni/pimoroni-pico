add_executable(
  explorerencoder
  demo.cpp
)

pico_generate_pio_header(explorerencoder ${CMAKE_CURRENT_LIST_DIR}/quadrature_out.pio)

# Pull in pico libraries that we need
target_link_libraries(explorerencoder pico_stdlib pico_explorer encoder-pio)

# create map/bin/hex file etc.
pico_add_extra_outputs(explorerencoder)