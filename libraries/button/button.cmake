add_library(button INTERFACE)

target_sources(button INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/button.cpp
)

target_include_directories(button INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(button INTERFACE pico_stdlib)