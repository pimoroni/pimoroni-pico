add_library(yukon INTERFACE)

target_sources(yukon INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/yukon.cpp
  ${CMAKE_CURRENT_LIST_DIR}/logging.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/led_strip/led_strip.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/quad_servo/quad_servo_direct.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/quad_servo/quad_servo_reg.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/big_motor/big_motor.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/dual_motor/dual_motor.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/dual_switched/dual_switched.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/bench_power/bench_power.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/audio_amp/audio_amp.cpp
  ${CMAKE_CURRENT_LIST_DIR}/modules/proto/proto.cpp
)

target_include_directories(yukon INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(yukon INTERFACE pico_stdlib pico_graphics tca9555 hardware_adc plasma)
