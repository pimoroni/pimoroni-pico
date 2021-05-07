add_executable(
  scroll-text
  scroll-text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(scroll-text pico_stdlib pico_scroll)

# create map/bin/hex file etc.
pico_add_extra_outputs(scroll-text)
