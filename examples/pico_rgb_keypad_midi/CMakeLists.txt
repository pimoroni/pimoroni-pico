set(CMAKE_C_FLAGS_DEBUG "-O0 -g")

add_executable(
  midi_rgb_keypad
  midi_demo.cpp
  usb_descriptors.c
)
target_include_directories(pico_rgb_keypad INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(midi_rgb_keypad pico_rgb_keypad pico_stdlib tinyusb_device tinyusb_board)

# create map/bin/hex file etc.
pico_add_extra_outputs(midi_rgb_keypad)
