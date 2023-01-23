add_library(aps6404 INTERFACE)

target_sources(aps6404 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/aps6404.cpp)

pico_generate_pio_header(aps6404 ${CMAKE_CURRENT_LIST_DIR}/aps6404.pio)

target_include_directories(aps6404 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(aps6404 INTERFACE pico_stdlib hardware_pio hardware_dma)

