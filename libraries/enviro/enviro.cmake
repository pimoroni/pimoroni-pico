add_library(enviro INTERFACE)

pico_generate_pio_header(enviro ${CMAKE_CURRENT_LIST_DIR}/ov2640.pio)

target_sources(enviro INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/enviro.cpp
    ${CMAKE_CURRENT_LIST_DIR}/enviro_indoor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/enviro_urban.cpp
    ${CMAKE_CURRENT_LIST_DIR}/enviro_grow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/enviro_weather.cpp
    ${CMAKE_CURRENT_LIST_DIR}/enviro_camera.cpp
)

target_include_directories(enviro INTERFACE ${CMAKE_CURRENT_LIST_DIR})

set(LIBALGOBSEC_PATH ${CMAKE_CURRENT_LIST_DIR}/bsec/libalgobsec.a)

# Pull in pico libraries that we need
target_link_libraries(enviro INTERFACE
    ${LIBALGOBSEC_PATH}
    pico_stdlib
    hardware_adc
    hardware_dma
    hardware_pio
    hardware_pwm
    pimoroni_i2c
    pcf85063a
    bme68x
    bh1745
    ltr559
    bme280
    pwm
    analog
    )