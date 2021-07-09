set(MOD_NAME breakout_as7262)
BREAKOUT_MOD_DIR := $(USERMOD_DIR)

# Add our source files to the respective variables.
SRC_USERMOD += $(BREAKOUT_MOD_DIR)/${MOD_NAME}.c
SRC_USERMOD_CXX += $(BREAKOUT_MOD_DIR)/${MOD_NAME}.cpp

# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(BREAKOUT_MOD_DIR)
CXXFLAGS_USERMOD += -I$(BREAKOUT_MOD_DIR)

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++