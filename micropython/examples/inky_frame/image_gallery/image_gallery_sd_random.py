"""
An offline image gallery that displays a random image from your SD card
and updates on a timer.

Copy images to the root of your SD card by plugging it into a computer.

If you want to use your own images they must be the screen dimensions
(or smaller) and saved as *non-progressive* jpgs.

Make sure to uncomment the correct size for your display!
"""

# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY    # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
from machine import Pin, SPI
import jpegdec
import sdcard
import os
import inky_frame
import random

# how often to change image (in minutes)
UPDATE_INTERVAL = 60

# set up the display
graphics = PicoGraphics(DISPLAY)

# set up the SD card
sd_spi = SPI(0, sck=Pin(18, Pin.OUT), mosi=Pin(19, Pin.OUT), miso=Pin(16, Pin.OUT))
sd = sdcard.SDCard(sd_spi, Pin(22))
os.mount(sd, "/sd")

# Create a new JPEG decoder for our PicoGraphics
j = jpegdec.JPEG(graphics)


def display_image(filename):

    # Open the JPEG file
    j.open_file(filename)

    # Decode the JPEG
    j.decode(0, 0, jpegdec.JPEG_SCALE_FULL)

    # Display the result
    graphics.update()


inky_frame.led_busy.on()

# Get a list of files that are in the directory
files = os.listdir("/sd")
# remove files from the list that aren't .jpgs or .jpegs
files = [f for f in files if f.endswith(".jpg") or f.endswith(".jpeg")]

while True:
    # pick a random file
    file = files[random.randrange(len(files))]

    # Open the file
    print(f"Displaying /sd/{file}")
    display_image("/sd/" + file)

    # Sleep or wait for a bit
    print(f"Sleeping for {UPDATE_INTERVAL} minutes")
    inky_frame.sleep_for(UPDATE_INTERVAL)
