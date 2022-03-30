add_library(servo2040 INTERFACE)

target_include_directories(servo2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(servo2040 INTERFACE pico_stdlib plasma servo servo_cluster)