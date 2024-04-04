from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY, PEN_P8
import pngdec

# Create a PicoGraphics instance
display = PicoGraphics(display=DISPLAY_PICO_DISPLAY, pen_type=PEN_P8, rotate=270)

# Set the backlight so we can see it!
display.set_backlight(1.0)

# Create an instance of the PNG Decoder
png = pngdec.PNG(display)

# Create some pens for use later.
BG = display.create_pen(200, 200, 200)

# 16 Reds
for i in range(16):
    display.create_pen(i * 16, 0, 0)

# 16 Greens
for i in range(16):
    display.create_pen(0, i * 16, 0)

# 16 Blues
for i in range(16):
    display.create_pen(0, 0, i * 16)

# Clear the screen
display.set_pen(BG)
display.clear()

try:
    # Open our PNG File from flash. In this example we're using an image of a cartoon pencil.
    # You can use Thonny to transfer PNG Images to your Pico.
    png.open_file("pencil_gray.png")

    # Decode our PNG file and set the X and Y
    png.decode(35, 10, scale=2, mode=pngdec.PNG_COPY, palette_offset=0)
    png.decode(35, 90, scale=2, mode=pngdec.PNG_COPY, palette_offset=16)
    png.decode(35, 170, scale=2, mode=pngdec.PNG_COPY, palette_offset=32)

# Handle the error if the image doesn't exist on the flash.
except OSError:
    print("Error: PNG File missing. Copy the PNG file from the example folder to your Pico using Thonny and run the example again.")

display.update()

# We're not doing anything else with the display now but we want to keep the program running!
while True:
    pass
