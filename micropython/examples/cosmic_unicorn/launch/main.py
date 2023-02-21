import time
import machine
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

# overclock to 200Mhz
machine.freq(200000000)

# create cosmic object and graphics surface for drawing
cosmic = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

brightness = 0.5


# returns the id of the button that is currently pressed or
# None if none are
def pressed():
    if cosmic.is_pressed(CosmicUnicorn.SWITCH_A):
        return CosmicUnicorn.SWITCH_A
    if cosmic.is_pressed(CosmicUnicorn.SWITCH_B):
        return CosmicUnicorn.SWITCH_B
    if cosmic.is_pressed(CosmicUnicorn.SWITCH_C):
        return CosmicUnicorn.SWITCH_C
    if cosmic.is_pressed(CosmicUnicorn.SWITCH_D):
        return CosmicUnicorn.SWITCH_D
    return None


# wait for a button to be pressed and load that effect
while True:
    graphics.set_font("bitmap6")
    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()
    graphics.set_pen(graphics.create_pen(155, 155, 155))
    graphics.text("PRESS", 3, 6, -1, 1)
    graphics.text("A B C OR D!", 5, 14, 32, 1, 0)

    # brightness up/down
    if cosmic.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        brightness += 0.01
    if cosmic.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        brightness -= 0.01
    brightness = max(min(brightness, 1.0), 0.0)

    cosmic.set_brightness(brightness)
    cosmic.update(graphics)

    if pressed() == CosmicUnicorn.SWITCH_A:
        import fire as effect
        break
    if pressed() == CosmicUnicorn.SWITCH_B:
        import supercomputer as effect  # noqa: F811
        break
    if pressed() == CosmicUnicorn.SWITCH_C:
        import rainbow as effect        # noqa: F811
        break
    if pressed() == CosmicUnicorn.SWITCH_D:
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

    sleep_pressed = cosmic.is_pressed(CosmicUnicorn.SWITCH_SLEEP)
    if sleep_pressed and not was_sleep_pressed:
        sleep = not sleep

    was_sleep_pressed = sleep_pressed

    if sleep:
        # fade out if screen not off
        cosmic.set_brightness(cosmic.get_brightness() - 0.01)

        if cosmic.get_brightness() > 0.0:
            effect.draw()

        # update the display
        cosmic.update(graphics)
    else:
        effect.draw()

        # update the display
        cosmic.update(graphics)

        # brightness up/down
        if cosmic.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
            brightness += 0.01
        if cosmic.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
            brightness -= 0.01
        brightness = max(min(brightness, 1.0), 0.0)

        cosmic.set_brightness(brightness)

    # pause for a moment (important or the USB serial device will fail
    time.sleep(0.001)
