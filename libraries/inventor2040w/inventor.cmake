add_library(inventor INTERFACE)

target_sources(inventor INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/inventor.cpp
)

target_include_directories(inventor INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(inventor INTERFACE pico_stdlib hardware_pwm hardware_pio hardware_i2c pimoroni_i2c motor servo encoder plasma pimoroni_i2c)