add_library(ms51xb9ae INTERFACE)

target_sources(ms51xb9ae INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/ms51xb9ae.cpp
)

target_include_directories(ms51xb9ae INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(ms51xb9ae INTERFACE pico_stdlib hardware_i2c)