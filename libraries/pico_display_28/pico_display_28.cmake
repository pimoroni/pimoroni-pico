add_library(pico_display_28 INTERFACE)

target_sources(pico_display_28 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_display_28.cpp
)

target_include_directories(pico_display_28 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_display_28 INTERFACE pico_stdlib)