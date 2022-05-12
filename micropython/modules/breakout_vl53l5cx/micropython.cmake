add_library(usermod_vl53l5cx INTERFACE)

target_sources(usermod_vl53l5cx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/vl53l5cx.c
    ${CMAKE_CURRENT_LIST_DIR}/vl53l5cx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/vl53l5cx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/platform.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/src/VL53L5CX_ULD_API/src/vl53l5cx_api.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/src/VL53L5CX_ULD_API/src/vl53l5cx_plugin_motion_indicator.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/src/VL53L5CX_ULD_API/src/vl53l5cx_plugin_detection_thresholds.c
)

target_include_directories(usermod_vl53l5cx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/vl53l5cx/src/VL53L5CX_ULD_API/inc
)

target_compile_definitions(usermod_vl53l5cx INTERFACE
    MODULE_VL53L5CX_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_vl53l5cx)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/vl53l5cx.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)