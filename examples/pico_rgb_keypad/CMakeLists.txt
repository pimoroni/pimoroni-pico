add_executable(
  rgb_keypad
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(rgb_keypad pico_stdlib pico_rgb_keypad)

# create map/bin/hex file etc.
pico_add_extra_outputs(rgb_keypad)
