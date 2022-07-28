# This simple example shows how to read and write from the SD card on Inky Frame.
from machine import Pin, SPI
import sdcard
import os

# set up the SD card
sd_spi = SPI(0, sck=Pin(18, Pin.OUT), mosi=Pin(19, Pin.OUT), miso=Pin(16, Pin.OUT))
sd = sdcard.SDCard(sd_spi, Pin(22))
os.mount(sd, "/sd")

# create a file and add some text
# if this file already exists it will be overwritten
with open("/sd/inkytest.txt", "w") as f:
    f.write("Hello Inky!\r\n")
    f.close()

# open the file and append some text (useful for logging!)
with open("/sd/inkytest.txt", "a") as f:
    f.write("We're appending some text\r\n")
    f.close()

# read the file and print the contents to the console
with open("/sd/inkytest.txt", "r") as f:
    data = f.read()
    print(data)
    f.close()
