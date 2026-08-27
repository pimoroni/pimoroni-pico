import time
import machine

user_button = machine.Pin(23, machine.Pin.IN)

while True:

    if user_button.value() == 0:
        print("BUTTON PRESSED!")

    time.sleep(0.2)
