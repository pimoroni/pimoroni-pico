# Cody Tolene
# Apache License 2.0
# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi

import uasyncio
import time


async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()
    x = width  # Start from the right edge

    # Brightness and color settings
    brightness_levels = [0.3, 0.6, 1.0]
    colors = [
        (255, 255, 255),  # White
        (255, 0, 0),  # Red
        (0, 255, 0),  # Green
        (0, 0, 255),  # Blue
        (255, 255, 0),  # Yellow
        (0, 255, 255),  # Light Blue
        (255, 0, 255),  # Pink
    ]
    brightness_index = 1  # Start in the middle at 0.6
    color_index = 0  # Start with white

    while True:
        # Handle button presses
        if picoUnicorn.is_pressed(picoUnicorn.BUTTON_Y):
            brightness_index = (brightness_index + 1) % len(brightness_levels)
            await uasyncio.sleep(0.2)  # Debounce delay

        if picoUnicorn.is_pressed(picoUnicorn.BUTTON_B):
            color_index = (color_index + 1) % len(colors)
            await uasyncio.sleep(0.2)  # Debounce delay

        graphics.set_pen(0)
        graphics.clear()

        current_time = time.localtime()
        time_str = "{:02}:{:02}:{:02}".format(
            current_time[3],  # Hour
            current_time[4],  # Minute
            current_time[5],  # Seconds
        )

        # Adjust color brightness
        current_color = colors[color_index]
        brightness = brightness_levels[brightness_index]
        adjusted_color = tuple(int(c * brightness) for c in current_color)

        graphics.set_pen(graphics.create_pen(*adjusted_color))

        # Scroll the text to the left
        graphics.text(time_str, x, 0, scale=1)
        picoUnicorn.update(graphics)

        # Update the x position
        text_width = graphics.measure_text(time_str, scale=1)
        x -= 1  # Move left by 1 pixel
        if (
            x < -text_width
        ):  # Reset x position when the text has completely scrolled off the screen
            x = width

        await uasyncio.sleep(0.4)  # Adjust the speed of scrolling


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
