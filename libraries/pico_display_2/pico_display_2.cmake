include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/st7789/st7789.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../pico_graphics/pico_graphics.cmake)

set(LIB_NAME pico_display_2)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE pico_stdlib hardware_spi hardware_pwm hardware_dma st7789 pico_graphics)