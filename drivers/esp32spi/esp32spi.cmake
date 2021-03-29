set(DRIVER_NAME esp32spi)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${DRIVER_NAME}.cpp
  ${CMAKE_CURRENT_LIST_DIR}/spi_drv.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ip_address.cpp
)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${DRIVER_NAME} INTERFACE pico_stdlib hardware_spi)