cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(
  display
  demo.cpp
  image_data.cpp
  pico_display.cpp
  ../../../drivers/st7789/st7789.cpp
  ../../../libraries/pico_graphics/pico_graphics.cpp
)

# Pull in pico libraries that we need
target_link_libraries(display pico_stdlib hardware_spi hardware_pwm hardware_dma)

# create map/bin/hex file etc.
pico_add_extra_outputs(display)
