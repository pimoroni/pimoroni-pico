# Inky Frame 5.7"
add_executable(
  inky_frame_day_planner
  inky_frame_day_planner.cpp
)

# Pull in pico libraries that we need
target_link_libraries(inky_frame_day_planner pico_stdlib inky_frame hardware_pwm hardware_spi hardware_i2c fatfs sdcard pico_graphics)

pico_enable_stdio_usb(inky_frame_day_planner 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(inky_frame_day_planner)

# Inky Frame 7.3"
add_executable(
  inky_frame_7_day_planner
  inky_frame_day_planner.cpp
)

# Pull in pico libraries that we need
target_link_libraries(inky_frame_7_day_planner pico_stdlib inky_frame_7 hardware_pwm hardware_spi hardware_i2c fatfs sdcard pico_graphics)

pico_enable_stdio_usb(inky_frame_7_day_planner 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(inky_frame_7_day_planner)

target_compile_definitions(inky_frame_7_day_planner PUBLIC INKY_FRAME_7)

