add_library(rv3028 INTERFACE)

target_sources(rv3028 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/rv3028.cpp
)

target_include_directories(rv3028 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(rv3028 INTERFACE pico_stdlib hardware_i2c)
