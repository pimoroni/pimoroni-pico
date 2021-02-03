add_executable(
  rtc_display
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(rtc_display pico_stdlib pico_display rv3028)

# create map/bin/hex file etc.
pico_add_extra_outputs(rtc_display)
