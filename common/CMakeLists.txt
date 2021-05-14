add_library(pimoroni_i2c INTERFACE)

target_sources(pimoroni_i2c INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pimoroni_i2c.cpp)

target_include_directories(pimoroni_i2c INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pimoroni_i2c INTERFACE pico_stdlib)
