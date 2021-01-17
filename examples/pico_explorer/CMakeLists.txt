add_executable(
  explorer
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(explorer pico_stdlib pico_explorer)

# create map/bin/hex file etc.
pico_add_extra_outputs(explorer)