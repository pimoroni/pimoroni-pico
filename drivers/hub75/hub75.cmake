add_library(hub75 INTERFACE)

target_sources(hub75 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hub75.cpp)

pico_generate_pio_header(hub75 ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)

target_include_directories(hub75 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(hub75 INTERFACE pico_stdlib hardware_pio hardware_dma pico_graphics)

