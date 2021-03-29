ESP32SPI_MOD_DIR := $(USERMOD_DIR)

# Add our source files to the respective variables.
SRC_USERMOD += $(ESP32SPI_MOD_DIR)/esp32spi.c
SRC_USERMOD_CXX += $(ESP32SPI_MOD_DIR)/esp32spi.cpp

# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(ESP32SPI_MOD_DIR)
CXXFLAGS_USERMOD += -I$(ESP32SPI_MOD_DIR)

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++