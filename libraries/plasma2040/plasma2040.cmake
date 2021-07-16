add_library(plasma2040 INTERFACE)

target_sources(plasma2040 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/apa102.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.cpp
)

target_include_directories(plasma2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(plasma2040 INTERFACE
    pico_stdlib
    hardware_pio
    hardware_dma
    )

pico_generate_pio_header(plasma2040 ${CMAKE_CURRENT_LIST_DIR}/apa102.pio)
pico_generate_pio_header(plasma2040 ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)