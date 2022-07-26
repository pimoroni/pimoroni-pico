add_executable(
  galactic_unicorn_demo
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(galactic_unicorn_demo pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(galactic_unicorn_demo 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(galactic_unicorn_demo)



add_executable(
  rainbow
  rainbow.cpp
)

# Pull in pico libraries that we need
target_link_libraries(rainbow pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(rainbow 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(rainbow)



add_executable(
  nostalgia_prompt
  nostalgia_prompt.cpp
)

# Pull in pico libraries that we need
target_link_libraries(nostalgia_prompt pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(nostalgia_prompt 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(nostalgia_prompt)



add_executable(
  eighties_super_computer
  eighties_super_computer.cpp
)

# Pull in pico libraries that we need
target_link_libraries(eighties_super_computer pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(eighties_super_computer 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(eighties_super_computer)




add_executable(
  fire_effect
  fire_effect.cpp
)

# Pull in pico libraries that we need
target_link_libraries(fire_effect pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(fire_effect 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(fire_effect)




add_executable(
  scroll_text
  scroll_text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(scroll_text pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(scroll_text 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(scroll_text)


add_executable(
  balls
  balls.cpp
)

# Pull in pico libraries that we need
target_link_libraries(balls pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(balls 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(balls)



add_executable(
  lava_lamp
  lava_lamp.cpp
)

# Pull in pico libraries that we need
target_link_libraries(lava_lamp pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(lava_lamp 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(lava_lamp)



add_executable(
  feature_test
  feature_test.cpp
  audio_samples.cpp
)

# Pull in pico libraries that we need
target_link_libraries(feature_test pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(feature_test 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(feature_test)