add_library(pico_scroll INTERFACE)

target_sources(pico_scroll INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_scroll.cpp
)

target_include_directories(pico_scroll INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_scroll INTERFACE pico_stdlib hardware_i2c)