add_library(adcfft 
    ${CMAKE_CURRENT_LIST_DIR}/adcfft.cpp
)

target_include_directories(adcfft INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(adcfft pico_stdlib hardware_pio hardware_dma hardware_adc hardware_irq)