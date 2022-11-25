import WIFI_CONFIG

if WIFI_CONFIG.SSID == "":
    import badger2040w
    badger2040w.init()
    badger2040w.screen.set_pen(15)
    badger2040w.screen.clear()
    badger2040w.screen.set_pen(0)
    badger2040w.screen.text("Please configure your WiFi in WIFI_CONFIG.py")
    badger2040w.screen.update()
