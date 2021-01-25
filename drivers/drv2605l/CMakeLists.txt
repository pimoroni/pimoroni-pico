add_library(drv2605l INTERFACE)

target_sources(drv2605l INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/drv2605l.cpp
)

target_include_directories(drv2605l INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(drv2605l INTERFACE pico_stdlib hardware_i2c)