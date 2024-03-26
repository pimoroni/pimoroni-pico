from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
import pngdec

# Create a PicoGraphics instance
graphics = PicoGraphics(DISPLAY)

# Set the font
graphics.set_font("bitmap8")

# Create an instance of the PNG Decoder
png = pngdec.PNG(graphics)

# Clear the screen
graphics.set_pen(1)
graphics.clear()
graphics.set_pen(0)

# Few lines of text.
graphics.text("Display PNG Image...", 10, 40, 300, 3)
graphics.text("Success!", 10, 70, 300, 3)

# Open our PNG File from flash. In this example we're using a green check mark.
# You can use Thonny to transfer PNG Images to your Inky Frame.
png.open_file("success.png")

# Decode our PNG file and set the X and Y
png.decode(130, 70)

# Start the screen update
graphics.update()
