if(NOT TARGET plasma)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/plasma/plasma.cmake)
endif()

if(NOT TARGET servo)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/servo/servo.cmake)
endif()

if(NOT TARGET servo_cluster)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/servo/servo_cluster.cmake)
endif()

add_library(servo2040 INTERFACE)

target_include_directories(servo2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(servo2040 INTERFACE pico_stdlib plasma servo servo_cluster)