set(LIB_NAME inky_frame)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(${LIB_NAME} INTERFACE hardware_i2c pico_graphics hardware_spi hardware_pwm bitmap_fonts hershey_fonts pico_stdlib sdcard fatfs pcf85063a uc8159 jpegdec)
