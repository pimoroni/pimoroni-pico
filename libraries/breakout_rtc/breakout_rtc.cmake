if(NOT TARGET rv3028)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/rv3028/rv3028.cmake)
endif()

set(LIB_NAME breakout_rtc)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE pico_stdlib rv3028)
