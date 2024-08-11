if(NOT TARGET plasma)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/plasma/plasma.cmake)
endif()

add_library(plasma_stick INTERFACE)

target_include_directories(plasma_stick INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(plasma_stick INTERFACE pico_stdlib plasma)