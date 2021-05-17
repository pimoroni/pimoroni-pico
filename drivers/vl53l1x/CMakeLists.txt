add_library(vl53l1x INTERFACE)

target_sources(vl53l1x INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/vl53l1x.cpp
)

target_include_directories(vl53l1x INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(vl53l1x INTERFACE pico_stdlib hardware_i2c pimoroni_i2c)
