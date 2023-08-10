import time
from picographics import PicoGraphics, DISPLAY_PICO_W_EXPLORER
from servo import Servo
from pimoroni import Button, Analog, Speaker
from machine import Pin

display = PicoGraphics(display=DISPLAY_PICO_W_EXPLORER)

adc0 = Analog(26)
adc1 = Analog(27)
adc2 = Analog(28)

button_a = Button(6)
button_b = Button(7)
button_x = Button(10)
button_y = Button(11)

speaker = Speaker(22)
amp_en = Pin(8, Pin.OUT)
amp_en.value(1)

servo0 = Servo(12)
servo1 = Servo(13)
servo2 = Servo(14)
servo3 = Servo(15)
servo0.enable()
servo1.enable()
servo2.enable()
servo3.enable()

# constants
BG = display.create_pen(32, 32, 64)
WHITE = display.create_pen(255, 255, 255)

ADC0_PEN = display.create_pen(255, 0, 0)
ADC1_PEN = display.create_pen(0, 255, 0)
ADC2_PEN = display.create_pen(0, 0, 255)
VOLUME = 0.3

i = 1

display.set_font("bitmap8")

while True:
    display.set_pen(BG)
    display.clear()

    # Read the ADCs
    adc0v = int(adc0.read_voltage() / 3.3 * 120)
    adc1v = int(adc1.read_voltage() / 3.3 * 120)
    adc2v = int(adc2.read_voltage() / 3.3 * 120)

    # ADC labels
    display.set_pen(WHITE)
    display.text("ADC0:", 20, 20, 100)
    display.text("ADC1:", 20, 40, 100)
    display.text("ADC2:", 20, 60, 100)

    # ADC graph
    display.set_pen(ADC0_PEN)
    display.circle(90 + adc0v, 26, 10)

    display.set_pen(ADC1_PEN)
    display.circle(90 + adc1v, 46, 10)

    display.set_pen(ADC2_PEN)
    display.circle(90 + adc2v, 66, 10)

    # On-board A/B/X/Y buttons
    if button_a.is_pressed:
        display.set_pen(WHITE)
        display.text("Button A pressed", 20, 110, 200)

    elif button_b.is_pressed:
        display.set_pen(WHITE)
        display.text("Button B pressed", 20, 110, 200)

    elif button_x.is_pressed and button_y.is_pressed:
        display.set_pen(WHITE)
        display.text("Buttons X and Y pressed", 20, 110, 200)

    elif button_x.is_pressed:
        display.set_pen(WHITE)
        display.text("Button X pressed", 20, 110, 200)

    elif button_y.is_pressed:
        display.set_pen(WHITE)
        display.text("Button Y pressed", 20, 110, 200)

    else:
        # no button press was detected
        display.set_pen(WHITE)
        display.text("Press a button!", 20, 110, 200)

    speaker.set_tone(i, VOLUME)

    # servo shenanigans
    if i > 600:
        display.text("Servo 0: Forwards", 20, 150, 200)
        servo0.to_min()
    else:
        display.text("Servo 0: Backwards", 20, 150, 200)
        servo0.to_max()
    if i > 600:
        display.text("Servo 1: Forwards", 20, 170, 200)
        servo1.to_min()
    else:
        display.text("Servo 1: Backwards", 20, 170, 200)
        servo1.to_max()
    if i > 600:
        display.text("Servo 2: Forwards", 20, 190, 200)
        servo2.to_min()
    else:
        display.text("Servo 2: Backwards", 20, 190, 200)
        servo2.to_max()
    if i > 600:
        display.text("Servo 3: Forwards", 20, 210, 200)
        servo3.to_min()
    else:
        display.text("Servo 3: Backwards", 20, 210, 200)
        servo3.to_max()

    i = i + 20
    if i > 1000:
        i = 1

    display.update()
    time.sleep(0.01)
