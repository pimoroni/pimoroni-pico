set(OUTPUT_NAME badger2040w_rtc)
add_executable(${OUTPUT_NAME} badger2040w_rtc.cpp)

target_link_libraries(${OUTPUT_NAME}
        badger2040w
        hardware_spi
        pico_cyw43_arch_lwip_threadsafe_background
)

target_include_directories(badger2040w_rtc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password")

if ("${WIFI_SSID}" STREQUAL "")
    error("WIFI_SSID is not defined")
elseif ("${WIFI_PASSWORD}" STREQUAL "")
    error("WIFI_PASSWORD is not defined")
else()
    target_compile_definitions(badger2040w_rtc PRIVATE 
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    )
endif()

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
