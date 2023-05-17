set(OUTPUT_NAME dv_slideshow)

add_executable(
  ${OUTPUT_NAME}
  dv_slideshow.cpp
)

target_compile_definitions(${OUTPUT_NAME} PRIVATE
  SDCARD_SPI_BUS=spi1
  SDCARD_PIN_SPI0_CS=15
  SDCARD_PIN_SPI0_SCK=10
  SDCARD_PIN_SPI0_MOSI=11
  SDCARD_PIN_SPI0_MISO=12)

# Pull in pico libraries that we need
target_link_libraries(${OUTPUT_NAME} pico_stdlib dv_display hardware_i2c hardware_uart pico_graphics fatfs jpegdec sdcard)

pico_enable_stdio_usb(${OUTPUT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${OUTPUT_NAME})
