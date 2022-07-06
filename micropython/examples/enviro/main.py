import time
import network
import secrets  # have a secrets.py file on your Pico W with an SSID and PASSWORD
from enviro import EnviroIndoor

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(secrets.SSID, secrets.PASSWORD)

# Wait for connect or fail
max_wait = 10
while max_wait > 0:
    if wlan.status() < 0 or wlan.status() >= 3:
        break
    max_wait -= 1
    print('waiting for connection...')
    time.sleep(1)

# Handle connection error
if wlan.status() != 3:
    raise RuntimeError('network connection failed')
else:
    print('connected')
    status = wlan.ifconfig()
    print('ip = ' + status[0])

indoor = EnviroIndoor()

print(",".join(indoor.readings().keys()))

while True:
    if indoor.take_reading():
        readings = indoor.readings()

        print(",".join([str(v) for v in indoor.readings().values()]))

    time.sleep(1)
