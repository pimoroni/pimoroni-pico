add_library(bh1745 INTERFACE)

target_sources(bh1745 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/bh1745.cpp)

target_include_directories(bh1745 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(bh1745 INTERFACE pico_stdlib hardware_i2c pimoroni_i2c)
