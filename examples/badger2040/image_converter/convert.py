#!/usr/bin/env python3

# converts images into a format suitable for display on badger2040. this
# includes scaling the image fit the longest edge, cropping down to 296x128
# and reducing to black and white with dither. the data is then output as an
# array that can be embedded directly into your c++ code

import io
import argparse
from PIL import Image, ImageEnhance
from pathlib import Path
try:
    import data_to_py
except ImportError as error:
    pass

parser = argparse.ArgumentParser(description='Converts images into the format used by Badger2040.')
parser.add_argument('file', nargs="+", help='input files to convert')
parser.add_argument('--out_dir', type=Path, default=None, help='output directory')
parser.add_argument('--binary', action="store_true", help='output binary file for MicroPython')
parser.add_argument('--py', action="store_true", help='output .py file for MicroPython embedding')
parser.add_argument('--resize', action="store_true", help='force images to 296x128 pixels')

options = parser.parse_args()


def convert_image(img):
    if options.resize:
        img = img.resize((296, 128)) # resize
    enhancer = ImageEnhance.Contrast(img)
    img = enhancer.enhance(2.0)
    img = img.convert("1") # convert to black and white
    return img


# create map of images based on input filenames
for input_filename in options.file:
    with Image.open(input_filename) as img:
        img = convert_image(img)

        image_name = Path(input_filename).stem

        w, h = img.size

        output_data = [~b & 0xff for b in list(img.tobytes())]

        if options.binary:
            if options.out_dir is not None:
                output_filename = (options.out_dir / image_name).with_suffix(".bin")
            else:
                output_filename = Path(input_filename).with_suffix(".bin")
            print(f"Saving to {output_filename}, {w}x{h}")
            with open(output_filename, "wb") as out:
                out.write(bytearray(output_data))
        elif options.py:
            if options.out_dir is not None:
                output_filename = (options.out_dir / image_name).with_suffix(".py")
            else:
                output_filename = Path(input_filename).with_suffix(".py")
            print(f"Saving to {output_filename}, {w}x{h}")
            with open(output_filename, "w") as out:
                data_to_py.write_stream(io.BytesIO(bytes(output_data)), out)
        else:
            image_code = '''\
static const uint8_t {image_name}[{count}] = {{
    {byte_data}
}};
    '''.format(image_name=image_name, count=len(output_data), byte_data=", ".join(str(b) for b in output_data))

            print(image_code)


