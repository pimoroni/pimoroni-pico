add_library(motor2040 INTERFACE)

target_include_directories(motor2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(motor2040 INTERFACE pico_stdlib plasma motor motor_cluster encoder)