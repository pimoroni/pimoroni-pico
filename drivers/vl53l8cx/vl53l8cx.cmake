add_library(vl53l8cx INTERFACE)

target_sources(vl53l8cx INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/vl53l8cx.cpp
  ${CMAKE_CURRENT_LIST_DIR}/platform.c
  ${CMAKE_CURRENT_LIST_DIR}/src/VL53L8CX_ULD_API/src/vl53l8cx_api.c
  ${CMAKE_CURRENT_LIST_DIR}/src/VL53L8CX_ULD_API/src/vl53l8cx_plugin_detection_thresholds.c
)

target_include_directories(vl53l8cx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src/VL53L8CX_ULD_API/inc
)

# Pull in pico libraries that we need
target_link_libraries(vl53l8cx INTERFACE pico_stdlib hardware_i2c)
