add_executable(
  inky_frame_jpeg_image
  inky_frame_jpeg_image.cpp
)

# Pull in pico libraries that we need
target_link_libraries(inky_frame_jpeg_image pico_stdlib jpegdec inky_frame fatfs hardware_pwm hardware_spi hardware_i2c fatfs sdcard pico_graphics)

pico_enable_stdio_usb(inky_frame_jpeg_image 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(inky_frame_jpeg_image)


add_executable(
  inky_frame_7_jpeg_image
  inky_frame_jpeg_image.cpp
)

# Pull in pico libraries that we need
target_link_libraries(inky_frame_7_jpeg_image pico_stdlib jpegdec inky_frame_7 fatfs hardware_pwm hardware_spi hardware_i2c fatfs sdcard pico_graphics)

pico_enable_stdio_usb(inky_frame_7_jpeg_image 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(inky_frame_7_jpeg_image)

target_compile_definitions(inky_frame_7_jpeg_image PUBLIC INKY_FRAME_7)
