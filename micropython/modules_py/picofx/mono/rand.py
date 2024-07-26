# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

import random
from picofx import Updatable

class RandomFX(Updatable):
    def __init__(self, period=0.05, low=0.0, high=1.0):
        self.period = period
        self.low = low
        self.high = high
        self.__time = 0
        self.__brightness = random.uniform(self.low, self.high)
    
    def __call__(self):
        return self.__brightness

    def tick(self, delta_ms):
        self.__time += delta_ms
        
        # Check if the dim duration has elapsed            
        if self.__time >= (self.period * 1000):
            self.__time -= (self.period * 1000)

            self.__brightness = random.uniform(self.low, self.high)
