add_library(as7262 INTERFACE)

target_sources(as7262 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/as7262.cpp
)

target_include_directories(as7262 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(as7262 INTERFACE pico_stdlib hardware_i2c)