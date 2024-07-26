# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

import math
from picofx import Cycling

class PulseFX(Cycling):
    def __init__(self, speed=1, phase=0):
        super().__init__(speed)
        self.phase = phase

    def __call__(self):
        angle = (self.__offset + self.phase) * math.pi * 2
        return (math.sin(angle) + 1) / 2.0

