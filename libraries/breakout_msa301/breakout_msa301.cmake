if(NOT TARGET msa301)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/msa301/msa301.cmake)
endif()

set(LIB_NAME breakout_msa301)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE pico_stdlib msa301 pimoroni_i2c)
