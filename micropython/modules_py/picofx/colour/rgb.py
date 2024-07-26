# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

class RGBFX:
    def __init__(self, red=255, green=255, blue=255):
        self.red = red
        self.green = green
        self.blue = blue

    def __call__(self):
        return max(min(self.red, 255), 0), \
               max(min(self.green, 255), 0), \
               max(min(self.blue, 255), 0)
