import time
import machine
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

# overclock to 200Mhz
machine.freq(200000000)

# create galactic object and graphics surface for drawing
galactic = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)

brightness = 0.5


# returns the id of the button that is currently pressed or
# None if none are
def pressed():
    if galactic.is_pressed(GalacticUnicorn.SWITCH_A):
        return GalacticUnicorn.SWITCH_A
    if galactic.is_pressed(GalacticUnicorn.SWITCH_B):
        return GalacticUnicorn.SWITCH_B
    if galactic.is_pressed(GalacticUnicorn.SWITCH_C):
        return GalacticUnicorn.SWITCH_C
    if galactic.is_pressed(GalacticUnicorn.SWITCH_D):
        return GalacticUnicorn.SWITCH_D
    return None


# wait for a button to be pressed and load that effect
while True:
    graphics.set_font("bitmap6")
    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()
    graphics.set_pen(graphics.create_pen(155, 155, 155))
    graphics.text("PRESS", 12, -1, -1, 1)
    graphics.text("A B C OR D!", 2, 5, -1, 1)

    # brightness up/down
    if galactic.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        brightness += 0.01
    if galactic.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        brightness -= 0.01
    brightness = max(min(brightness, 1.0), 0.0)

    galactic.set_brightness(brightness)
    galactic.update(graphics)

    if pressed() == GalacticUnicorn.SWITCH_A:
        import fire as effect
        break
    if pressed() == GalacticUnicorn.SWITCH_B:
        import supercomputer as effect  # noqa: F811
        break
    if pressed() == GalacticUnicorn.SWITCH_C:
        import rainbow as effect        # noqa: F811
        break
    if pressed() == GalacticUnicorn.SWITCH_D:
        import retroprompt as effect    # noqa: F811
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

    sleep_pressed = galactic.is_pressed(GalacticUnicorn.SWITCH_SLEEP)
    if sleep_pressed and not was_sleep_pressed:
        sleep = not sleep

    was_sleep_pressed = sleep_pressed

    if sleep:
        # fade out if screen not off
        galactic.set_brightness(galactic.get_brightness() - 0.01)

        if galactic.get_brightness() > 0.0:
            effect.draw()

        # update the display
        galactic.update(graphics)
    else:
        effect.draw()

        # update the display
        galactic.update(graphics)

        # brightness up/down
        if galactic.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
            brightness += 0.01
        if galactic.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
            brightness -= 0.01
        brightness = max(min(brightness, 1.0), 0.0)

        galactic.set_brightness(brightness)

    # pause for a moment (important or the USB serial device will fail
    time.sleep(0.001)
