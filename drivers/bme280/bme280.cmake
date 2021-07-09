set(DRIVER_NAME bme280)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/bme280.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/bme280.c)

target_include_directories(${DRIVER_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${DRIVER_NAME} INTERFACE pico_stdlib hardware_i2c pimoroni_i2c)

target_compile_definitions(${DRIVER_NAME} INTERFACE
    BME280_FLOAT_ENABLE
)
