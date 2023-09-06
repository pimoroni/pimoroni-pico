if(NOT TARGET st7567)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/st7567/st7567.cmake)
endif()

if(NOT TARGET button)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/button/button.cmake)
endif()

if(NOT TARGET rgbled)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/rgbled/rgbled.cmake)
endif()

if(NOT TARGET pico_graphics)
  include(${CMAKE_CURRENT_LIST_DIR}/../pico_graphics/pico_graphics.cmake)
endif()

set(LIB_NAME gfx_pack)
add_library(${LIB_NAME} INTERFACE)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE pico_stdlib st7567 button rgbled)