cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(
  unicorn
  demo.cpp
  pico_unicorn.cpp
)

pico_generate_pio_header(unicorn ${CMAKE_CURRENT_LIST_DIR}/unicorn.pio)

# Pull in pico libraries that we need
target_link_libraries(unicorn pico_stdlib hardware_pio hardware_dma)

# create map/bin/hex file etc.
pico_add_extra_outputs(unicorn)
