set(DRIVER_NAME pcf85063a)
add_library(${DRIVER_NAME} INTERFACE)

target_sources(${DRIVER_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${DRIVER_NAME}.cpp)

target_include_directories(${DRIVER_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Include datetime_t for cross-compatibility with RP2350 (no RTC) boards
# TODO: We should migrate away from using this non-standard type
target_compile_definitions(${DRIVER_NAME} INTERFACE PICO_INCLUDE_RTC_DATETIME=1)

# Pull in pico libraries that we need
target_link_libraries(${DRIVER_NAME} INTERFACE pico_stdlib pico_util hardware_i2c pimoroni_i2c)
