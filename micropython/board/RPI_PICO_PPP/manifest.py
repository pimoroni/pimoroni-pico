include("$(PORT_DIR)/boards/manifest.py")

require("bundle-networking")

include("../manifest_pico.py")

freeze("../../modules_py", "lte.py")