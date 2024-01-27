if(NOT TARGET uc8151)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/uc8151/uc8151.cmake)
endif()

if(NOT TARGET pico_graphics)
  include(${CMAKE_CURRENT_LIST_DIR}/../../libraries/pico_graphics/pico_graphics.cmake)
endif()

set(LIB_NAME badger2040w)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE uc8151 pico_graphics pico_stdlib hardware_pwm)
