set(MOD_NAME breakout_pmw3901)
PICOSCROLL_MOD_DIR := $(USERMOD_DIR)

# Add our source files to the respective variables.
SRC_USERMOD += $(PICOSCROLL_MOD_DIR)/${MOD_NAME}.c
SRC_USERMOD_CXX += $(PICOSCROLL_MOD_DIR)/${MOD_NAME}.cpp

# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(PICOSCROLL_MOD_DIR)
CXXFLAGS_USERMOD += -I$(PICOSCROLL_MOD_DIR)

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++