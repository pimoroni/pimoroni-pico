add_library(pico_display INTERFACE)

target_sources(pico_display INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_display.cpp
)

target_include_directories(pico_display INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_display INTERFACE pico_stdlib)