set(DRIVER_NAME mlx90640)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/src/functions/MLX90640_API.cpp
  ${CMAKE_CURRENT_LIST_DIR}/MLX90640_RP2040_I2C_Driver.cpp
  ${CMAKE_CURRENT_LIST_DIR}/mlx90640.cpp
)

target_link_libraries(${DRIVER_NAME} INTERFACE pico_stdlib hardware_i2c pimoroni_i2c)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/headers)
