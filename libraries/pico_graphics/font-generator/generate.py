#!/usr/bin/env python3

import sys, math
from PIL import Image

if len(sys.argv) < 2:
  print("Usage: ./generate input-file")
  sys.exit()

input_file  = sys.argv[1]

font_image = Image.open(input_file)

char_count = 222
bytes_per_row = math.ceil(font_image.width / 8)
char_height = int(font_image.height / char_count)
max_char_width = int(font_image.width)

char_start_end = []

# calculate character positions
for i in range(0, char_count):
  char_start = max_char_width
  char_end = 0

  for y in range(0, char_height):
    for x in range(0, max_char_width):
      if font_image.getpixel((x, y + (i * char_height))) == (0, 0, 0):
        char_start = min(char_start, x)
        char_end = max(char_end, x)

  char_start_end.append((char_start, char_end))

print("#include <cstdint>")
print("uint8_t font_data[] = {")
print("  {0}, // bytes per row".format(bytes_per_row))
print("  {0}, // character height".format(char_height))

for i in range(0, char_count):
  char_width = char_start_end[i][1] - char_start_end[i][0] + 1
  if char_width < 0:
    char_width = 0

  if i + 32 < 128:
    character = chr(i + 32)
  else:
    character = str(i + 32)

  # avoid messing up the c output...
  if character == "/":
    character = "forward slash"

  print("  {0}, // {1}".format(char_width, character))
  for y in range(0, char_height):
    print("  ", end="")
    bit = 0
    for x in range(0, char_width):
      if bit % 8 == 0:
        if x != 0:
          print(", ", end="")
        print("0b", end="")

      if font_image.getpixel((x + char_start_end[i][0], y + (i * char_height))) == (255, 255, 255):
        print("0", end="")
      else:
        print("1", end="")

      bit += 1

    for xpad in range(bit, bytes_per_row * 8):
      if bit % 8 == 0:
        if xpad != 0:
          print(", ", end="")
        print("0b", end="")

      print("0", end="")

      bit += 1

    print(",")

print("};")
