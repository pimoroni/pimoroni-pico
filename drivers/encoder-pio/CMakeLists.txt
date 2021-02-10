add_library(encoder-pio INTERFACE)

target_sources(encoder-pio INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/encoder.cpp
  ${CMAKE_CURRENT_LIST_DIR}/capture.cpp
)

pico_generate_pio_header(encoder-pio ${CMAKE_CURRENT_LIST_DIR}/encoder.pio)

target_include_directories(encoder-pio INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(encoder-pio INTERFACE pico_stdlib hardware_pio)