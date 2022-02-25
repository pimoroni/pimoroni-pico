import badger2040
import machine
import time

display = badger2040.Badger2040()
display.update_speed(badger2040.UPDATE_TURBO)
display.pen(15)
display.clear()
display.update()

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)

message = None
message_y = 60


def button(pin):
    global message
    if message is not None:
        return
    if pin == button_a:
        message = "Button A"
        return
    if pin == button_b:
        message = "Button B"
        return
    if pin == button_c:
        message = "Button C"
        return
    if pin == button_up:
        message = "Button U"
        return
    if pin == button_down:
        message = "Button D"
        return


button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_b.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_c.irq(trigger=machine.Pin.IRQ_RISING, handler=button)

button_up.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_down.irq(trigger=machine.Pin.IRQ_RISING, handler=button)


while True:
    if message is not None:
        display.pen(15)
        display.clear()
        display.pen(0)
        display.thickness(4)
        display.text(message, 10, message_y, 2.0)
        for _ in range(2):
            display.update()
        message = None
    time.sleep(0.1)
