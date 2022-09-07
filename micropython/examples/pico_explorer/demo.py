import time
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER
from motor import Motor
from pimoroni import Button, Analog, Buzzer

display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)

adc0 = Analog(26)
adc1 = Analog(27)
adc2 = Analog(28)

button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)

BG = display.create_pen(32, 32, 64)
WHITE = display.create_pen(255, 255, 255)

ADC0_PEN = display.create_pen(255, 0, 0)
ADC1_PEN = display.create_pen(0, 255, 0)
ADC2_PEN = display.create_pen(0, 0, 255)

MOTOR1 = Motor((8, 9))
MOTOR2 = Motor((10, 11))

BUZZER = Buzzer(0)

i = 1

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
        display.text("Plug a jumper wire from GP0 to AUDIO to hear noise!", 20, 110, 200)

    BUZZER.set_tone(i)

    # Motors
    if i > 600:
        display.text("Motor 1: Forwards", 20, 180, 200)
        MOTOR1.full_positive()
    else:
        display.text("Motor 1: Backwards", 20, 180, 200)
        MOTOR1.full_negative()

    if i > 600:
        display.text("Motor 2: Forwards", 20, 200, 200)
        MOTOR2.full_positive()
    else:
        display.text("Motor 2: Backwards", 20, 200, 200)
        MOTOR2.full_negative()

    i = i + 20
    if i > 1000:
        i = 1

    display.update()
    time.sleep(0.01)
