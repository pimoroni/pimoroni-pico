add_library(msa301 INTERFACE)

target_sources(msa301 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/msa301.cpp
)

target_include_directories(msa301 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(msa301 INTERFACE pico_stdlib hardware_i2c pimoroni_i2c)