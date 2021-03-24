add_library(st7789
    ${CMAKE_CURRENT_LIST_DIR}/st7789.cpp)

target_include_directories(st7789 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(st7789 pico_stdlib hardware_spi hardware_pwm hardware_dma)
