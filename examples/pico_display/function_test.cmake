add_executable(
  function_test
  function_test.cpp
)

# Pull in pico libraries that we need
target_link_libraries(function_test pico_stdlib rgbled pico_display pico_graphics st7789 button)

# create map/bin/hex file etc.
pico_add_extra_outputs(function_test)