if(NOT TARGET sdcard)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/sdcard/sdcard.cmake)
endif()

if(NOT TARGET fatfs)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/fatfs/fatfs.cmake)
endif()

if(NOT TARGET pcf85063a)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/pcf85063a/pcf85063a.cmake)
endif()

if(NOT TARGET psram_display)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/psram_display/psram_display.cmake)
endif()

if(NOT TARGET inky73)
  include(${CMAKE_CURRENT_LIST_DIR}/../../drivers/inky73/inky73.cmake)
endif()

if(NOT TARGET jpegdec)
  include(${CMAKE_CURRENT_LIST_DIR}/../jpegdec/jpegdec.cmake)
endif()

if(NOT TARGET pico_graphics)
  include(${CMAKE_CURRENT_LIST_DIR}/../pico_graphics/pico_graphics.cmake)
endif()

set(LIB_NAME inky_frame_7)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE hardware_i2c pico_graphics hardware_spi hardware_pwm bitmap_fonts hershey_fonts pico_stdlib sdcard fatfs pcf85063a psram_display inky73 jpegdec)

target_compile_options(${LIB_NAME} INTERFACE -Wno-error=reorder)