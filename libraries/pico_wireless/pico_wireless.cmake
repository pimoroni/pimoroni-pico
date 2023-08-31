if(NOT TARGET esp32spi)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/esp32spi/esp32spi.cmake)
endif()

add_library(pico_wireless INTERFACE)

target_sources(pico_wireless INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_wireless.cpp
)

target_include_directories(pico_wireless INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_wireless INTERFACE pico_stdlib hardware_spi esp32spi)
