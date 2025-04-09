add_library(duo75 INTERFACE)

target_sources(duo75 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/duo75.cpp)

pico_generate_pio_header(duo75 ${CMAKE_CURRENT_LIST_DIR}/duo75.pio)

target_include_directories(duo75 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(duo75 INTERFACE pico_stdlib hardware_pio hardware_dma pico_graphics)

