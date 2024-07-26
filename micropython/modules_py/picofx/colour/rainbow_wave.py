# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

from picofx import Cycling
from .hsv import rgb_from_hsv

class RainbowWaveSequenceFX(Cycling):
    def __init__(self, speed=1, size=1, sat=1, val=1):
        super().__init__(speed)
        self.size = size
        self.sat = sat
        self.val = val

    def __call__(self, pos):
        def fx():
            nonlocal pos
            phase = pos / self.size
            percent = (self.__offset + phase) % 1.0
            r, g, b = rgb_from_hsv(percent, self.sat, self.val)
            return int(r * 255), int(g * 255), int(b * 255)
        return self, fx
