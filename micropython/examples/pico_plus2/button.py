from machine import Pin

# Setup the LED and Button pin.
led = Pin(25, Pin.OUT)
button = Pin(45, Pin.IN)

# Light the LED when the button is presed!
while True:

    if button.value() == 0:
        led.value(1)
    else:
        led.value(0)
