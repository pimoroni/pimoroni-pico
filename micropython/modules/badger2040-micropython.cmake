include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

include(pimoroni_i2c/micropython)
include(pimoroni_bus/micropython)

include(micropython-common-breakouts)

include(hershey_fonts/micropython)
include(bitmap_fonts/micropython)

include(badger2040/micropython)
if(NOT DEFINED BADGER2040_NO_MODULES)
include(micropython/examples/badger2040/micropython-builtins)
endif()
include(plasma/micropython)
include(ulab/code/micropython)
include(qrcode/micropython/micropython)
