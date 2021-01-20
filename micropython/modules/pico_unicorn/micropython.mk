PICOUNICORN_MOD_DIR := $(USERMOD_DIR)

# Add our source files to the respective variables.
SRC_USERMOD += $(PICOUNICORN_MOD_DIR)/pico_unicorn.c
SRC_USERMOD_CXX += $(PICOUNICORN_MOD_DIR)/pico_unicorn.cpp

# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(PICOUNICORN_MOD_DIR)
CXXFLAGS_USERMOD += -I$(PICOUNICORN_MOD_DIR)

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++