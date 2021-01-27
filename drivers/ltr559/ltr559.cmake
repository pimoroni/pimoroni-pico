add_library(ltr559 INTERFACE)

target_sources(ltr559 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ltr559.cpp)

target_include_directories(ltr559 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(ltr559 INTERFACE pico_stdlib hardware_i2c)