# Hershey font converter

This is the tool that was used to create the fonts.hpp file that is included
in the Badger2040 library. It is included in case you want to convert other
Hershey fonts - we only include a subset to keep the flash image to a
reasonable size.

It takes in `.jhf` files (which can be found in the `fonts` directory) and
converts them into C++ arrays and structures that can be embedded directly
into your code.

Credit to Paul Bourke for another fantastic file format guide which provided a
lot of the necessary detail to make this work - read more about how the
Hershey fonts work at http://paulbourke.net/dataformats/hershey/. While you're
there make sure to check out some of his other file format guides, they are
a wealth of useful information.