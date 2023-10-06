# Image Gallery <!-- omit in toc -->

- [Image transfer instructions](#image-transfer-instructions)
  - [image\_gallery.py](#image_gallerypy)
  - [image\_gallery\_sd.py / image\_gallery\_sd\_random.py](#image_gallery_sdpy--image_gallery_sd_randompy)
- [Image Credits](#image-credits)

Some example programs to display images on your Inky Frame, plus sample images in different sizes.

Use:
640x400 for Inky Frame 4.0"
600x448 for Inky Frame 5.7"
800x480 for Inky Frame 7.3"

If you want to use your own images, they will need to be the correct dimensions for your screen and saved *without progressive encoding*.

## Image transfer instructions

In all cases the images will need to be copied to the root of your Pico W or SD card.

### image_gallery.py

Copy the images to your Pico W using Thonny.

### image_gallery_sd.py / image_gallery_sd_random.py

Pop an SD card into your computer to copy the images across.

Alternatively, you can transfer them using Thonny, but you will have to mount the SD card using the REPL first:

```python
import os
import sdcard
from machine import Pin
sd_spi = SPI(0, sck=Pin(18, Pin.OUT), mosi=Pin(19, Pin.OUT), miso=Pin(16, Pin.OUT))
sd = sdcard.SDCard(sd_spi, Pin(22))
os.mount(sd, "/sd")
```

## Image Credits

Sample images from the Webb Space Telescope (credit: NASA, ESA, CSA, and STScI).
Find more gorgeous images and info @ https://webbtelescope.org/

... and Raspberry Pi <3
