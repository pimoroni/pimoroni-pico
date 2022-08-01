include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

# Essential
include(pimoroni_i2c/micropython)
include(pimoroni_bus/micropython)

# Pico Graphics Essential
include(hershey_fonts/micropython)
include(bitmap_fonts/micropython)
include(picographics/micropython)

# Pico Graphics Extra
include(jpegdec/micropython)
include(qrcode/micropython/micropython)

# Sensors & Breakouts
include(breakout_dotmatrix/micropython)
include(breakout_encoder/micropython)
include(breakout_ioexpander/micropython)
include(breakout_ltr559/micropython)
include(breakout_as7262/micropython)
include(breakout_rgbmatrix5x5/micropython)
include(breakout_matrix11x7/micropython)
include(breakout_msa301/micropython)
include(breakout_pmw3901/micropython)
include(breakout_mics6814/micropython)
include(breakout_potentiometer/micropython)
include(breakout_rtc/micropython)
include(breakout_trackball/micropython)
include(breakout_sgp30/micropython)
include(breakout_bh1745/micropython)
include(breakout_bme68x/micropython)
include(breakout_bme280/micropython)
include(breakout_bmp280/micropython)
include(breakout_icp10125/micropython)
include(breakout_scd41/micropython)
include(breakout_vl53l5cx/micropython)
include(pcf85063a/micropython)

# Utility
include(adcfft/micropython)
include(wakeup/micropython)

# LEDs & Matrices
include(plasma/micropython)
include(hub75/micropython)

# Packs
include(pico_unicorn/micropython)
include(pico_scroll/micropython)
include(pico_rgb_keypad/micropython)

# Servos & Motors
include(pwm/micropython)
include(servo/micropython)
include(encoder/micropython)
include(motor/micropython)

# include(micropython-common)

include(modules_py/modules_py)