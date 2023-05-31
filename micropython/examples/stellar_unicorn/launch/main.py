import time
import math
import machine
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY

# overclock to 200Mhz
machine.freq(200000000)

# create stellar object and graphics surface for drawing
stellar = StellarUnicorn()
graphics = PicoGraphics(DISPLAY)

brightness = 0.5


# returns the id of the button that is currently pressed or
# None if none are
def pressed():
    if stellar.is_pressed(StellarUnicorn.SWITCH_A):
        return StellarUnicorn.SWITCH_A
    if stellar.is_pressed(StellarUnicorn.SWITCH_B):
        return StellarUnicorn.SWITCH_B
    if stellar.is_pressed(StellarUnicorn.SWITCH_C):
        return StellarUnicorn.SWITCH_C
    if stellar.is_pressed(StellarUnicorn.SWITCH_D):
        return StellarUnicorn.SWITCH_D
    return None


# wait for a button to be pressed and load that effect
while True:
    b = int((math.sin(time.ticks_ms() / 200) + 1) / 2.0 * 255)
    b = max(60, b)

    graphics.set_font("bitmap6")
    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()

    graphics.set_pen(graphics.create_pen(b, 0, 0))
    graphics.pixel(0, 3)
    graphics.set_pen(graphics.create_pen(0, b, 0))
    graphics.pixel(0, 5)
    graphics.set_pen(graphics.create_pen(0, 0, b))
    graphics.pixel(0, 7)
    graphics.set_pen(graphics.create_pen(b, 0, b))
    graphics.pixel(0, 9)

    # brightness up/down
    if stellar.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
        brightness += 0.01
    if stellar.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
        brightness -= 0.01
    brightness = max(min(brightness, 1.0), 0.0)

    stellar.set_brightness(brightness)
    stellar.update(graphics)

    if pressed() == StellarUnicorn.SWITCH_A:
        import fire as effect
        break
    if pressed() == StellarUnicorn.SWITCH_B:
        import supercomputer as effect  # noqa: F811
        break
    if pressed() == StellarUnicorn.SWITCH_C:
        import rainbow as effect        # noqa: F811
        break
    if pressed() == StellarUnicorn.SWITCH_D:
        import today as effect    # noqa: F811
        break

    # pause for a moment
    time.sleep(0.01)

# wait until all buttons are released
while pressed() is not None:
    time.sleep(0.1)

effect.graphics = graphics
effect.init()

sleep = False
was_sleep_pressed = False


# wait
while True:
    # if A, B, C, or D are pressed then reset
    if pressed() is not None:
        machine.reset()

    sleep_pressed = stellar.is_pressed(StellarUnicorn.SWITCH_SLEEP)
    if sleep_pressed and not was_sleep_pressed:
        sleep = not sleep

    was_sleep_pressed = sleep_pressed

    if sleep:
        # fade out if screen not off
        stellar.set_brightness(stellar.get_brightness() - 0.01)

        if stellar.get_brightness() > 0.0:
            effect.draw()

        # update the display
        stellar.update(graphics)
    else:
        effect.draw()

        # update the display
        stellar.update(graphics)

        # brightness up/down
        if stellar.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
            brightness += 0.01
        if stellar.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
            brightness -= 0.01
        brightness = max(min(brightness, 1.0), 0.0)

        stellar.set_brightness(brightness)

    # pause for a moment (important or the USB serial device will fail
    time.sleep(0.001)
