include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(micropython-common)
enable_ulab()

# C++ Magic Memory
include(cppmem/micropython)

# Drop the C++ demangler, which nothing can reach
include(cxx_terminate/micropython)

# SP/CE screens on the connector. The GC heap lives in PSRAM here, so the displays'
# region is the SRAM it leaves free.
find_package(SPIDISPLAY CONFIG REQUIRED)
