set(DRIVER_NAME ov2640)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/ov2640.cpp
)

pico_generate_pio_header(${DRIVER_NAME} ${CMAKE_CURRENT_LIST_DIR}/ov2640.pio)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${DRIVER_NAME} INTERFACE pimoroni_i2c hardware_pwm hardware_pio hardware_dma hardware_irq)