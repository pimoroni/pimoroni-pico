# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

from picofx import Cycling

class BlinkFX(Cycling):
    def __init__(self, speed=1, phase=0.0, duty=0.5):
        super().__init__(speed)
        self.phase = phase
        self.duty = duty

    def __call__(self):
        percent = (self.__offset + self.phase) % 1.0
        return 1.0 if percent < self.duty else 0.0


class BlinkSequenceFX(Cycling):
    def __init__(self, speed=1, size=1, duty=0.5):
        super().__init__(speed)
        self.size = size
        self.duty = duty

    def __call__(self, pos):
        def fx():
            nonlocal pos
            phase = (pos / self.size)
            percent = (self.__offset + phase) % 1.0
            return 1.0 if percent < self.duty else 0.0
        return self, fx

