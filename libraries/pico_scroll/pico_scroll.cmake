if(NOT TARGET pico_graphics)
  include(${CMAKE_CURRENT_LIST_DIR}/../pico_graphics/pico_graphics.cmake)
endif()

add_library(pico_scroll INTERFACE)

set(PICO_SCROLL_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/pico_scroll.cpp
  ${CMAKE_CURRENT_LIST_DIR}/pico_scroll_font.cpp)

target_sources(pico_scroll INTERFACE
  ${PICO_SCROLL_SOURCES}
)

target_include_directories(pico_scroll INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico_scroll INTERFACE pico_stdlib pico_graphics hardware_i2c)