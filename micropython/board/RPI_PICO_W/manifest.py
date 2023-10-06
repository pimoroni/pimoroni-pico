include("$(PORT_DIR)/boards/manifest.py")

require("bundle-networking")

# SD Card
require("sdcard")

# Bluetooth
require("aioble")

include("../manifest_picow.py")