#!/usr/bin/env python3

# converts images into a format suitable for display on badger2040. this
# includes scaling the image fit the longest edge, cropping down to 296x128
# and reducing to black and white with dither. the data is then output as an
# array that can be embedded directly into your c++ code

import argparse, sys, os, glob
from PIL import Image, ImageEnhance
from pathlib import Path

parser = argparse.ArgumentParser(
  description='Converts images into the format used by Badger2040.')
parser.add_argument('file', nargs="+", help='input files to convert')

options = None
try:
  options = parser.parse_args()
except:
  parser.print_help()
  sys.exit(0)

def convert_image(img):
  img = img.resize((296, 128)) # resize and crop
  enhancer = ImageEnhance.Contrast(img)
  img = enhancer.enhance(2.0)
  img = img.convert("1") # convert to black and white
  return img


# create map of images based on input filenames
for input_filename in options.file:
  with Image.open(input_filename) as img:
    img = convert_image(img)

    image_name = Path(input_filename).stem

    h, w = img.size

    data = Image.Image.getdata(img)

    bytes = []
    byte = 0
    byte_idx = 0
    x = 0
    for v in data:
      byte <<= 1
      byte |= 1 if v == 0 else 0
      byte_idx += 1

      if byte_idx == 8: # next byte...
        bytes.append(str(byte))
        byte_idx = 0
        byte = 0

    image_code = '''\
static const uint8_t {image_name}[{count}] = {{
  {byte_data}
}};
  '''.format(image_name=image_name, count=len(bytes), byte_data=", ".join(bytes))

    print(image_code)


