if(NOT TARGET motor)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/motor/motor.cmake)
endif()

if(NOT TARGET servo)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/servo/servo.cmake)
endif()

if(NOT TARGET encoder)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/encoder/encoder.cmake)
endif()

if(NOT TARGET plasma)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/plasma/plasma.cmake)
endif()

add_library(inventor INTERFACE)

target_sources(inventor INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/inventor.cpp
)

target_include_directories(inventor INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(inventor INTERFACE pico_stdlib hardware_pwm hardware_pio hardware_i2c pimoroni_i2c motor servo encoder plasma)