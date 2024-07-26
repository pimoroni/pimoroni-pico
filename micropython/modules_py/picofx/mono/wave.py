# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

import math
from picofx import Cycling

class WaveSequenceFX(Cycling):
    def __init__(self, speed=1, size=1):
        super().__init__(speed)
        self.size = size

    def __call__(self, pos):
        def fx():
            nonlocal pos
            phase = pos / self.size
            angle = (self.__offset + phase) * math.pi * 2
            return (math.sin(angle) + 1) / 2.0
        return self, fx
