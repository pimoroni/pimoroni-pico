# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

import math
from picofx import Updatable

class BinarySequenceFX(Updatable):
    
    def __init__(self, period=0.1):
        self.period = period
        self.__time = 0
        self.__counter = 0
    
    def __call__(self, bit):
        def fx():
            nonlocal bit
            return 1.0 if self.__counter & 1 << bit else 0.0
        return self, fx

    def tick(self, delta_ms):
        self.__time += delta_ms
        
        # Check if the dim duration has elapsed            
        if self.__time >= (self.period * 1000):
            self.__time -= (self.period * 1000)
            
            self.__counter += 1
