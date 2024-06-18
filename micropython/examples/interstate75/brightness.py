from interstate75 import Interstate75
import jpegdec
import time

# Create the Interstate display
i75 = Interstate75(display=Interstate75.DISPLAY_INTERSTATE75_32X32)
graphics = i75.display

# Clear the screen
graphics.set_pen(graphics.create_pen(0, 0, 0))
graphics.clear()

# Load and render a jpeg
img = jpegdec.JPEG(graphics)
img.open_file("sun.jpg")
img.decode(0, 0, jpegdec.JPEG_SCALE_FULL, dither=False)

# Set up a brightness variance
delta = -1
brightness = 100

# And loop as we do it
while True:
    start = time.ticks_ms()

    i75.set_brightness(brightness)
    brightness = brightness + delta
    if brightness > 100 or brightness < 1:
        delta = delta * -1
    i75.update(graphics)

    print("total took: {} ms".format(time.ticks_ms() - start))
