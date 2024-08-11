add_executable(
  cosmic_rainbow_text
  cosmic_rainbow_text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(cosmic_rainbow_text pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics cosmic_unicorn)
pico_enable_stdio_usb(cosmic_rainbow_text 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cosmic_rainbow_text)



add_executable(
  cosmic_rainbow
  cosmic_rainbow.cpp
)

# Pull in pico libraries that we need
target_link_libraries(cosmic_rainbow pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics cosmic_unicorn)
pico_enable_stdio_usb(cosmic_rainbow 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cosmic_rainbow)




add_executable(
  cosmic_eighties_super_computer
  cosmic_eighties_super_computer.cpp
)

# Pull in pico libraries that we need
target_link_libraries(cosmic_eighties_super_computer pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics cosmic_unicorn)
pico_enable_stdio_usb(cosmic_eighties_super_computer 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cosmic_eighties_super_computer)




add_executable(
  cosmic_fire_effect
  cosmic_fire_effect.cpp
)

# Pull in pico libraries that we need
target_link_libraries(cosmic_fire_effect pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics cosmic_unicorn)
pico_enable_stdio_usb(cosmic_fire_effect 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cosmic_fire_effect)




add_executable(
  cosmic_scroll_text
  cosmic_scroll_text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(cosmic_scroll_text pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics cosmic_unicorn)
pico_enable_stdio_usb(cosmic_scroll_text 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cosmic_scroll_text)


add_executable(
  cosmic_lava_lamp
  cosmic_lava_lamp.cpp
)

# Pull in pico libraries that we need
target_link_libraries(cosmic_lava_lamp pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics cosmic_unicorn)
pico_enable_stdio_usb(cosmic_lava_lamp 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cosmic_lava_lamp)

