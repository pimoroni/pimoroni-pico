# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi
#
# Cody Tolene
# Apache License 2.0

import time
import math
import uasyncio

async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()
    height = picoUnicorn.get_height()
    while True:
        t = time.ticks_ms() / 1000
        for x in range(width):
            for y in range(height):
                # Calculate the wave effect using a combination of sine waves
                wave_x = math.sin((x / width * 2 * math.pi) + t)
                wave_y = math.sin((y / height * 2 * math.pi) + t)
                wave = (wave_x + wave_y) / 2
                
                # Normalize wave value to a smaller range to avoid very dark spots
                wave = (wave + 1) / 2 * 0.8 + 0.2

                # Map the wave value to a color gradient (e.g., shades of blue)
                blue = int(wave * 255)  # Normalize wave to 0-255
                green = int(wave * 127)  # Less intense green for a blueish look

                PEN = graphics.create_pen(0, green, blue)
                graphics.set_pen(PEN)
                graphics.pixel(x, y)

        # Ask the Unicorn to update the graphics
        picoUnicorn.update(graphics)

        # And sleep, so we update ~ 60fps
        await uasyncio.sleep(1.0 / 60)

# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK
    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
