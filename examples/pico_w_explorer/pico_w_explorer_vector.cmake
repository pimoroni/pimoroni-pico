add_executable(
  pico_w_explorer_vector
  pico_w_explorer_vector.cpp
)

# Pull in pico libraries that we need
target_link_libraries(pico_w_explorer_vector pico_stdlib pico_graphics pico_vector st7789)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_w_explorer_vector)
