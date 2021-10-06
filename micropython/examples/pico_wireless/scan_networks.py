import picowireless
import time

picowireless.init()

picowireless.start_scan_networks()

while True:
    networks = picowireless.get_scan_networks()
    print("Found {} network(s)...".format(networks))
    for network in range(networks):
        ssid = picowireless.get_ssid_networks(network)
        print("{}: {}".format(network, ssid))
    print("\n")
    time.sleep(10)
