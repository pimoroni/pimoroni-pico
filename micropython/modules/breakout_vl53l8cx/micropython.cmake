add_library(usermod_vl53l8cx INTERFACE)

target_sources(usermod_vl53l8cx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/vl53l8cx.c
    ${CMAKE_CURRENT_LIST_DIR}/vl53l8cx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/vl53l8cx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/platform.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/src/VL53L8CX_ULD_API/src/vl53l8cx_api.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/src/VL53L8CX_ULD_API/src/vl53l8cx_plugin_motion_indicator.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/src/VL53L8CX_ULD_API/src/vl53l8cx_plugin_detection_thresholds.c
)

target_include_directories(usermod_vl53l8cx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l8cx/src/VL53L8CX_ULD_API/inc
)

target_compile_definitions(usermod_vl53l8cx INTERFACE
    MODULE_VL53L8CX_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_vl53l8cx)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/vl53l8cx.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)