add_executable(
  rainbow_text
  rainbow_text.cpp
)

# Pull in pico libraries that we need
target_link_libraries(rainbow_text pico_stdlib hardware_pio hardware_adc hardware_dma pico_graphics galactic_unicorn)
pico_enable_stdio_usb(rainbow_text 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(rainbow_text)



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

# WiFi / Bluetooth examples, only built when the board type is set to PicoW
if (${PICO_BOARD} STREQUAL "pico_w")

  add_executable(
    scrolling_quote
    scrolling_quote.cpp
  )

  # Pull in the pico libraries that we need
  target_link_libraries(
    scrolling_quote
    pico_stdlib pico_graphics galactic_unicorn
    pico_cyw43_arch_lwip_threadsafe_background pico_lwip_http
  )

  # Need to include lwipopts.h from the local dir
  target_include_directories(
    scrolling_quote PRIVATE ${CMAKE_CURRENT_LIST_DIR}
  )

  # WiFi connection details; provide these to your CMake command!
  target_compile_definitions(
    scrolling_quote PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    CYW43_HOST_NAME=\"GalacticUnicorn\"
    PICO_CYW43_ARCH_DEBUG_ENABLED
  )

  pico_enable_stdio_usb(scrolling_quote 1)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(scrolling_quote)

endif ()
