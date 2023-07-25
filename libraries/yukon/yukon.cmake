add_library(yukon INTERFACE)

target_sources(yukon INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/yukon.cpp
)

target_include_directories(yukon INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(yukon INTERFACE pico_stdlib pico_graphics tca9555 hardware_adc)
