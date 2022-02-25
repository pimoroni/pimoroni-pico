#!/usr/bin/env python3

# converts Hershey fonts into a format easier to consume by the Badger2040
# e-ink badge library - the result can be piped directly into a .hpp file.

import argparse, sys, os, glob
from pathlib import Path

parser = argparse.ArgumentParser(
  description='Converts Hershey fonts into the format used by Badger2040.')
parser.add_argument('file', nargs="+", help='input files to convert')

options = None
try:
  options = parser.parse_args()
except:
  parser.print_help()
  sys.exit(0)

def convert_font(data):
  # parse hershey font data file, the file format contains one record per
  # character with the format:
  #
  #  2761  8MXP[OZPYQZQ[P]N_
  #
  # first 5 characters: represent the character id (or a dummy value
  # of 12345). if it is a character id then a separate file contains the
  # mapping information to convert that into an ascii table entry. if all of
  # the ids are 12345 then it seems that the files contain 95 entries that
  # start from ascii character 32 (space) in order
  #
  # next 3 characters: number of vertices (including special left/right
  # bounds value)
  #
  # next 2 characters: represent the "left" and "right" offsets for this
  # character, to convert you must subtract 'R' from them. this pair counts
  # as the first "vertex"
  #
  # subsequent pairs of characters: represent each vertex, as before converted
  # into numbers by subtracting 'R'. there are vertex count (-1) of these
  #
  # note! within the vertex data can be extra newline characters which must
  # be ignored.

  vertex_data = []
  char_data = {}

  auto_char_code = 0

  # we'll loop, extracting one character per iteration, until all of the
  # data is consumed
  while(len(data) > 0):
    # extract character id
    char_id = int(data[0:5])

    char_code = auto_char_code
    auto_char_code += 1

    # extract vertex count
    vertex_count = int(data[5:8])
    vertex_count -= 1 # remove one for the bounds data

    # extract bounds
    left = ord(data[8:9]) - ord("R")
    right = ord(data[9:10]) - ord("R")

    char_data[char_code] = {
      "width": right - left,
      "vertex_count": vertex_count,
      "first_vertex_offset": len(vertex_data)
    }

    # work out expected total length of character data
    char_data_len = 10 + (vertex_count * 2)

    # if there are any newlines within this data then remove them before we
    # move on the parse the vertices
    while data.find('\n', char_data_len) != -1:
      data = data.replace('\n', '', 1)

    for i in range(0, vertex_count):
      offset = 10 + (i * 2)
      if data[offset:offset + 2] == " R":
        # pen up for one
        x = -128
        y = -128
      else:
        x = ord(data[offset + 0:offset + 1]) - ord("R") - left
        y = ord(data[offset + 1:offset + 2]) - ord("R")

      vertex_data.append(str(x))
      vertex_data.append(str(y))

    data = data[char_data_len:]

  font_name = Path(input_filename).stem

  # create code for all of the vertices
  vertex_code = '''\
    static const int8_t {font_name}_vertices[{count}] = {{
      {vertices}
    }};
  '''.format(font_name=font_name, count=len(vertex_data), vertices=", ".join(vertex_data))

  # create code for chars and font
  #
  # struct hershey_font_glyph_t {
  #   uint32_t width;        // character width
  #   uint32_t vertex_count; // number of vertices
  #   int8_t  *vertices;     // vertex data (indices: even = x, odd = y)
  # };
  chars = []
  for i in range(0, 95):
    if i in char_data:
      chars.append(
        "      {{.width={width}, .vertex_count={vertex_count}, .vertices=&{font_name}_vertices[{offset}]}}".format(
        width=char_data[i]["width"],
        vertex_count=char_data[i]["vertex_count"],
        font_name=font_name,
        offset=char_data[i]["first_vertex_offset"]
      ))
    else:
      chars.append("      {.width=0, .vertex_count=0, .vertices=nullptr}")

  # struct hershey_font_t {
  #   hershey_font_glyph_t chars[95];
  # };
  font_code = '''\
    static const hershey_font_t {font_name} {{
      .chars = {{
  {chars}
      }}
    }};
  '''.format(font_name=font_name, chars=",\n".join(chars))

  print(vertex_code)

  print(font_code)

  return """  {{ \"{font_name}\", &{font_name} }}""".format(font_name=font_name)


# create map of font name to font structure
font_list = []
for input_filename in options.file:
  input_file = open(input_filename, mode='r')
  data = input_file.read()
  input_file.close()

  font_list.append(convert_font(data))

map_code = '''\
std::map<std::string, const hershey_font_t*> fonts = {{
{font_list}
}};
'''.format(font_list=",\n".join(font_list))

print(map_code)