add_library(ntp_client
        ${CMAKE_CURRENT_LIST_DIR}/ntp_client.cpp
        )
target_include_directories(ntp_client PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(ntp_client 
        pico_cyw43_arch_lwip_poll
        pico_stdlib)


set(OUTPUT_NAME inky_pack_clock_ntp)
add_executable(${OUTPUT_NAME} inky_pack_clock_ntp.cpp)

target_link_libraries(${OUTPUT_NAME} PRIVATE
        ntp_client
        pico_stdlib 
        pico_cyw43_arch_lwip_poll
        hardware_spi 
        hardware_pwm 
        hardware_dma  
        hardware_rtc
        button 
        uc8151 
        pico_graphics
)
target_include_directories(${OUTPUT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        )



target_compile_definitions(${OUTPUT_NAME} PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
