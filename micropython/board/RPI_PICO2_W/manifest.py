include("$(PORT_DIR)/boards/manifest.py")

require("bundle-networking")

# Bluetooth
require("aioble")

include("../manifest_pico2.py")

# The SP/CE screen library, from the spidisplay clone beside this one, frozen so one
# uf2 carries it
freeze("$(PORT_DIR)/../../../spidisplay/src")