# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

from picofx import Cycling

class EmergencyFX(Cycling):
    def __init__(self, speed=1, flashes=1, duty=0.5):
        super().__init__(speed)
        self.flashes = flashes
        self.duty = duty
        
    @property
    def flashes(self):
        return self.__flashes
    
    @flashes.setter
    def flashes(self, flashes):
        if not isinstance(flashes, int) or flashes <= 0:
            raise ValueError("flashes must be an integer greater than zero")

        self.__flashes = int(flashes)

    @property
    def duty(self):
        return self.__duty
    
    @duty.setter
    def duty(self, duty):
        self.__duty = max(min(duty, 1.0), 0.0)


    def __call__(self, side):
        if side == "left":
            def fx():
                if self.__offset < 0.5:
                    percent = (self.__offset * 2 * self.__flashes) % 1.0
                    return 1.0 if percent < self.__duty else 0.0
                return 0.0
            return self, fx

        if side == "right":
            def fx():
                if self.__offset >= 0.5:
                    percent = ((self.__offset - 0.5) * 2 * self.__flashes) % 1.0
                    return 1.0 if percent < self.__duty else 0.0
                return 0.0
            return self, fx

        raise ValueError("side should be `left` or `right`")
