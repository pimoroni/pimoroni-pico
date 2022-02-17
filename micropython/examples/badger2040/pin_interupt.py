import badger2040
import machine

display = badger2040.Badger2040()

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)

display.thickness(10)
display.pen(0)
display.line(0, 5, 295, 5)
display.line(0, 123, 295, 123)

display.thickness(1)
for x in range(14):
    display.line(x * 20, 10, x * 20, 118)

display.line(0, 0, 295, 127)
display.line(0, 127, 295, 0)

display.font("sans")
display.thickness(5)
display.text("Hello World", 10, 30, 1.0)
display.pen(7)
display.text("Hello World", 10, 60, 1.0)
display.pen(11)
display.text("Hello World", 10, 90, 1.0)

display.update()

dirty = False
pressed = None

def button(pin):
    global dirty, pressed
    if pin == button_a:
        pressed = "Button A"
        dirty = True
        return
    if pin == button_b:
        pressed = "Button B"
        dirty = True
        return

button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_b.irq(trigger=machine.Pin.IRQ_RISING, handler=button)

# This breaks Thonny, since it's no longer possible to Stop the code
# need to press the reset button on the board...
@micropython.asm_thumb
def lightsleep():
    wfi()

while True:
    if dirty:
        display.pen(15)
        display.clear()
        display.pen(0)
        display.text(pressed, 10, 60, 2.0)
        display.update()
        dirty = False
    lightsleep()


