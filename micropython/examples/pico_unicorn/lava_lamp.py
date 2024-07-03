# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi
#
# Cody Tolene
# Apache License 2.0

import uasyncio
import random

class Blob:
    def __init__(self, width, height, graphics):
        self.graphics = graphics
        self.width = width
        self.height = height
        self.x = random.uniform(0, width)
        self.y = random.uniform(0, height)
        self.dx = random.uniform(-0.5, 0.5)
        self.dy = random.uniform(-0.5, 0.5)
        self.color = graphics.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

    async def update(self):
        self.x += self.dx
        self.y += self.dy
        if self.x <= 0 or self.x >= self.width:
            self.dx *= -1
        if self.y <= 0 or self.y >= self.height:
            self.dy *= -1

        self.graphics.set_pen(self.color)
        self.graphics.circle(int(self.x), int(self.y), 3)

async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()
    height = picoUnicorn.get_height()
    blobs = [Blob(width, height, graphics) for _ in range(5)]

    while True:
        graphics.set_pen(graphics.create_pen(0, 0, 0))
        graphics.clear()

        for blob in blobs:
            await blob.update()

        picoUnicorn.update(graphics)
        await uasyncio.sleep(0.1)

# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK
    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
