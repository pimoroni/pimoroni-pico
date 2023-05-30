add_executable(
  stellar_rainbow_text
  stellar_rainbow_text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(stellar_rainbow_text pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics stellar_unicorn)
pico_enable_stdio_usb(stellar_rainbow_text 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(stellar_rainbow_text)



add_executable(
  stellar_rainbow
  stellar_rainbow.cpp
)

# Pull in pico libraries that we need
target_link_libraries(stellar_rainbow pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics stellar_unicorn)
pico_enable_stdio_usb(stellar_rainbow 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(stellar_rainbow)




add_executable(
  stellar_eighties_super_computer
  stellar_eighties_super_computer.cpp
)

# Pull in pico libraries that we need
target_link_libraries(stellar_eighties_super_computer pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics stellar_unicorn)
pico_enable_stdio_usb(stellar_eighties_super_computer 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(stellar_eighties_super_computer)




add_executable(
  stellar_fire_effect
  stellar_fire_effect.cpp
)

# Pull in pico libraries that we need
target_link_libraries(stellar_fire_effect pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics stellar_unicorn)
pico_enable_stdio_usb(stellar_fire_effect 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(stellar_fire_effect)




add_executable(
  stellar_scroll_text
  stellar_scroll_text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(stellar_scroll_text pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics stellar_unicorn)
pico_enable_stdio_usb(stellar_scroll_text 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(stellar_scroll_text)


add_executable(
  stellar_lava_lamp
  stellar_lava_lamp.cpp
)

# Pull in pico libraries that we need
target_link_libraries(stellar_lava_lamp pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics stellar_unicorn)
pico_enable_stdio_usb(stellar_lava_lamp 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(stellar_lava_lamp)

