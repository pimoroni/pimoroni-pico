from picographics import PicoGraphics, DISPLAY_TUFTY_2040
import pngdec

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

# Create an instance of the PNG Decoder
png = pngdec.PNG(display)

# Create some pens for use later.
BG = display.create_pen(200, 200, 200)
TEXT = display.create_pen(0, 0, 0)

# Clear the screen
display.set_pen(BG)
display.clear()

display.set_pen(TEXT)
display.text("PNG Pencil", 105, 80)

try:
    # Open our PNG File from flash. In this example we're using an image of a cartoon pencil.
    # You can use Thonny to transfer PNG Images to your Pico.
    png.open_file("pencil.png")

    # Decode our PNG file and set the X and Y
    png.decode(140, 100)

# Handle the error if the image doesn't exist on the flash.
except OSError:
    print("Error: PNG File missing. Copy the PNG file from the example folder to your Pico using Thonny and run the example again.")

display.update()

# We're not doing anything else with the display now but we want to keep the program running!
while True:
    pass
