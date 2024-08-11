if(NOT TARGET plasma)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/plasma/plasma.cmake)
endif()

if(NOT TARGET motor)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/motor/motor.cmake)
endif()

if(NOT TARGET motor_cluster)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/motor/motor_cluster.cmake)
endif()

if(NOT TARGET encoder)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/encoder/encoder.cmake)
endif()

add_library(motor2040 INTERFACE)

target_include_directories(motor2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(motor2040 INTERFACE pico_stdlib plasma motor motor_cluster encoder)