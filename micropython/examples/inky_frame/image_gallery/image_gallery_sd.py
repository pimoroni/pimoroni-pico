"""
An offline image gallery that switches between five jpg images on your SD card.

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

# you can change your file names here
IMAGE_A = "sd/jwst1.jpg"
IMAGE_B = "sd/jwst2.jpg"
IMAGE_C = "sd/jwst3.jpg"
IMAGE_D = "sd/jwst4.jpg"
IMAGE_E = "sd/jwst5.jpg"

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


# setup
print('Press a button to display an image!')

while True:
    inky_frame.button_a.led_off()
    inky_frame.button_b.led_off()
    inky_frame.button_c.led_off()
    inky_frame.button_d.led_off()
    inky_frame.button_e.led_off()

    if inky_frame.button_a.read():
        print('Refreshing image A.')
        inky_frame.button_a.led_on()
        display_image(IMAGE_A)
    elif inky_frame.button_b.read():
        print('Refreshing image B.')
        inky_frame.button_b.led_on()
        display_image(IMAGE_B)
    elif inky_frame.button_c.read():
        print('Refreshing image C.')
        inky_frame.button_c.led_on()
        display_image(IMAGE_C)
    elif inky_frame.button_d.read():
        print('Refreshing image D.')
        inky_frame.button_d.led_on()
        display_image(IMAGE_D)
    elif inky_frame.button_e.read():
        print('Refreshing image E.')
        inky_frame.button_e.led_on()
        display_image(IMAGE_E)

    # Go to sleep if on battery power
    inky_frame.turn_off()
