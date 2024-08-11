if(NOT TARGET pico_graphics)
  include(${CMAKE_CURRENT_LIST_DIR}/../pico_graphics/pico_graphics.cmake)
endif()

add_library(stellar_unicorn INTERFACE)

pico_generate_pio_header(stellar_unicorn ${CMAKE_CURRENT_LIST_DIR}/stellar_unicorn.pio)
pico_generate_pio_header(stellar_unicorn ${CMAKE_CURRENT_LIST_DIR}/audio_i2s.pio)


target_sources(stellar_unicorn INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/stellar_unicorn.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../pico_synth/pico_synth.cpp
)

target_include_directories(stellar_unicorn INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(stellar_unicorn INTERFACE pico_stdlib pico_graphics hardware_adc hardware_pio hardware_dma)
