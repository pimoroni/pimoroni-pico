add_library(vl53l5cx INTERFACE)

target_sources(vl53l5cx INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/vl53l5cx.cpp
  ${CMAKE_CURRENT_LIST_DIR}/platform.c
  ${CMAKE_CURRENT_LIST_DIR}/src/VL53L5CX_ULD_API/src/vl53l5cx_api.c
  ${CMAKE_CURRENT_LIST_DIR}/src/VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.c
)

target_include_directories(vl53l5cx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src/VL53L5CX_ULD_API/inc
)

# Pull in pico libraries that we need
target_link_libraries(vl53l5cx INTERFACE pico_stdlib hardware_i2c)
