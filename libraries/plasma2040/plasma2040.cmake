if(NOT TARGET plasma)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/plasma/plasma.cmake)
endif()

add_library(plasma2040 INTERFACE)

target_include_directories(plasma2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(plasma2040 INTERFACE pico_stdlib plasma)