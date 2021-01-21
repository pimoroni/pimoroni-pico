add_executable(
  rgbmatrix5x5
  demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(rgbmatrix5x5 pico_stdlib hardware_spi hardware_pwm hardware_dma is31fl3731)

# create map/bin/hex file etc.
pico_add_extra_outputs(rgbmatrix5x5)