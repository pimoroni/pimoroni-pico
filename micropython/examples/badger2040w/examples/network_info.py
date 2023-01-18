import badger2040w as badger2040
from badger2040w import WIDTH
import time
import network
import machine

TEXT_SIZE = 1
LINE_HEIGHT = 16

# Display Setup
display = badger2040.Badger2040W()
display.led(128)


# Button handler
def button(pin):

    time.sleep(0.01)
    if not pin.value():
        return

    if button_a.value() and button_c.value():
        machine.reset()


# Setup buttons
button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_c.irq(trigger=machine.Pin.IRQ_RISING, handler=button)

# Connects to the wireless network. Ensure you have entered your details in WIFI_CONFIG.py :).
display.connect()
net = network.WLAN(network.STA_IF).ifconfig()

# Page Header
display.set_pen(15)
display.clear()
display.set_pen(0)

display.set_pen(0)
display.rectangle(0, 0, WIDTH, 20)
display.set_pen(15)
display.text("badgerOS", 3, 4)
display.text("Network Details", WIDTH - display.measure_text("Network Details") - 4, 4)
display.set_pen(0)

y = 35 + int(LINE_HEIGHT / 2)

if net:
    display.text("> LOCAL IP: {}".format(net[0]), 0, y, WIDTH)
    y += LINE_HEIGHT
    display.text("> Subnet: {}".format(net[1]), 0, y, WIDTH)
    y += LINE_HEIGHT
    display.text("> Gateway: {}".format(net[2]), 0, y, WIDTH)
    y += LINE_HEIGHT
    display.text("> DNS: {}".format(net[3]), 0, y, WIDTH)
else:
    display.text("> No network connection!", 0, y, WIDTH)
    y += LINE_HEIGHT
    display.text("> Check details in WIFI_CONFIG.py", 0, y, WIDTH)

display.update()

# Call halt in a loop, on battery this switches off power.
# On USB, the app will exit when A+C is pressed because the launcher picks that up.
while True:
    display.halt()
