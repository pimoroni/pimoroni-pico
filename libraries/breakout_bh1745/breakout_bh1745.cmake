if(NOT TARGET bh1745)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/bh1745/bh1745.cmake)
endif()

set(LIB_NAME breakout_bh1745)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE pico_stdlib bh1745 pimoroni_i2c)
