include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(micropython-common)

# Tiny 2040 has 8MB flash with 1MB reserved for app,
# weighs about 612k without ulab
enable_ulab()

# C++ Magic Memory
include(cppmem/micropython)
