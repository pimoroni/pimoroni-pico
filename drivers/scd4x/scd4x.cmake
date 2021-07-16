set(DRIVER_NAME scd4x)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src/scd4x_i2c.c
  ${CMAKE_CURRENT_LIST_DIR}/src/sensirion_common.c
  ${CMAKE_CURRENT_LIST_DIR}/src/sensirion_i2c.c
  ${CMAKE_CURRENT_LIST_DIR}/i2c_hal.cpp
  ${CMAKE_CURRENT_LIST_DIR}/scd4x.cpp
)

target_link_libraries(${DRIVER_NAME} INTERFACE pimoroni_i2c hardware_i2c)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)