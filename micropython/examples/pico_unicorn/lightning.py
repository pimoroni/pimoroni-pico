# Cody Tolene
# Apache License 2.0
# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi

import uasyncio
import random


class Lightning:
    def __init__(self, width, height, graphics):
        self.graphics = graphics
        self.width = width
        self.height = height
        self.flash = False
        self.flash_duration = 0
        self.bolts = []

    def create_bolt(self):
        bolt = []
        x = random.randint(0, self.width - 1)
        y = 0
        while y < self.height:
            bolt.append((x, y))
            x += random.choice([-1, 0, 1])
            x = max(0, min(x, self.width - 1))
            y += 1
        return bolt

    async def update(self):
        if self.flash:
            self.graphics.set_pen(self.graphics.create_pen(255, 255, 255))
            for x in range(self.width):
                for y in range(self.height):
                    self.graphics.pixel(x, y)
            self.flash_duration -= 1
            if self.flash_duration <= 0:
                self.flash = False
                self.bolts = []
        else:
            if random.random() < 0.05:
                self.flash = True
                self.flash_duration = random.randint(1, 3)
                self.bolts = [self.create_bolt() for _ in range(random.randint(1, 3))]

        for bolt in self.bolts:
            for x, y in bolt:
                self.graphics.set_pen(self.graphics.create_pen(255, 255, 255))
                self.graphics.pixel(x, y)


async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()  # Get the width of the display
    height = picoUnicorn.get_height()  # Get the height of the display
    lightning = Lightning(width, height, graphics)  # Create Lightning effect

    while True:
        graphics.set_pen(graphics.create_pen(0, 0, 0))  # Clear the screen
        graphics.clear()

        await lightning.update()  # Update the Lightning effect

        picoUnicorn.update(graphics)  # Update the display
        await uasyncio.sleep(0.1)  # Pause for a short time before the next update


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
