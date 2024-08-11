add_library(hub75_legacy INTERFACE)

target_sources(hub75_legacy INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hub75.cpp)

target_include_directories(hub75_legacy INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(hub75_legacy INTERFACE pico_stdlib hardware_pio hardware_dma)

pico_generate_pio_header(hub75_legacy ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)