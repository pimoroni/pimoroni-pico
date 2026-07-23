from picographics import PicoGraphics, DISPLAY_I2C_OLED_128X128
import machine
import time

i2c = machine.I2C()

display = PicoGraphics(display=DISPLAY_I2C_OLED_128X128, bus=i2c)
WIDTH, HEIGHT = display.get_bounds()

display.set_font("bitmap8")

while True:
    # clear to black
    display.set_pen(0)  # Black
    display.clear()

    # write some text
    display.set_pen(15)
    display.text("Hello World!", 0, 0, scale=2)

    # draw some boxes showing all the 'pens' - AKA dither patterns - available for this mono screen
    for pen in range(16):
        display.set_pen(pen)
        display.rectangle(pen * 8, HEIGHT - 16, 8, 16)

    # add some labels
    display.set_pen(15)
    display.text("0", 0, HEIGHT - 28, scale=1)
    display.text("15", 116, HEIGHT - 28, scale=1)

    # update the screen
    display.update()

    time.sleep(1)
