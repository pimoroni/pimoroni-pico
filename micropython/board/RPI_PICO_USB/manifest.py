include("$(PORT_DIR)/boards/manifest.py")

include("../manifest_pico.py")

require("usb-device")
require("usb-device-hid")
require("usb-device-cdc")
require("usb-device-keyboard")
require("usb-device-mouse")
require("usb-device-midi")
