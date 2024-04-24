from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2, PEN_P8
import pngdec

# Create a PicoGraphics instance
display = PicoGraphics(display=DISPLAY_PICO_DISPLAY_2, pen_type=PEN_P8, rotate=270)

# Get the display width/height so we can position the PNGs
width, height = display.get_bounds()

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

# Adding in an white background colour at the beginning of each offset.
for i in range(3):
    display.update_pen(i * 16, 200, 200, 200)

# Clear the screen
display.set_pen(BG)
display.clear()


try:
    # Open our PNG File from flash. In this example we're using an image of a cartoon pencil.
    # You can use Thonny to transfer PNG Images to your Pico.
    png.open_file("pencil_gray.png")
    
    # Horizontally/vertically center the three PNG Images.
    png_w = png.get_width()
    png_h = png.get_height()
    
    offset_x = (width - png_w * 2) // 2
    height_y = (height // 3)
    offset_y = (height_y - png_h * 2) // 2

    # Decode our PNG file and set the X and Y
    png.decode(offset_x, offset_y, scale=2, mode=pngdec.PNG_COPY, palette_offset=0)
    png.decode(offset_x, offset_y + height_y, scale=2, mode=pngdec.PNG_COPY, palette_offset=16)
    png.decode(offset_x, offset_y + (height_y * 2), scale=2, mode=pngdec.PNG_COPY, palette_offset=32)

# Handle the error if the image doesn't exist on the flash.
except OSError:
    print("Error: PNG File missing. Copy the PNG file from the example folder to your Pico using Thonny and run the example again.")

display.update()

# We're not doing anything else with the display now but we want to keep the program running!
while True:
    pass
