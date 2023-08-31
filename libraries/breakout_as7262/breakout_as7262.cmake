if(NOT TARGET as7262)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/as7262/as7262.cmake)
endif()

set(LIB_NAME breakout_as7262)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE pico_stdlib hardware_i2c as7262)
