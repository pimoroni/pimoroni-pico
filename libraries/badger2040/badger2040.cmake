if(NOT TARGET uc8151_legacy)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/uc8151_legacy/uc8151_legacy.cmake)
endif()

if(NOT TARGET bitmap_fonts)
  include(${CMAKE_CURRENT_LIST_DIR}/../bitmap_fonts/bitmap_fonts.cmake)
endif()

if(NOT TARGET hershey_fonts)
  include(${CMAKE_CURRENT_LIST_DIR}/../hershey_fonts/hershey_fonts.cmake)
endif()

set(LIB_NAME badger2040)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE bitmap_fonts hershey_fonts uc8151_legacy pico_stdlib hardware_pwm)
