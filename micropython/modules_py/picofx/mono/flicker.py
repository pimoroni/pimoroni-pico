# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

from picofx import Updatable

class FlickerFX(Updatable):
    
    def __init__(self, dim_min=0.02, dim_max=0.04, lockout_min=0.05, lockout_max=0.1, high=1.0, low=0.5):
        self.dim_min = dim_min
        self.dim_max = dim_max
        self.lockout_min = lockout_min
        self.lockout_max = lockout_max
        self.high = high
        self.low = low
        
        self.__is_dim = False
        self.__dim_dur = 0        
        self.__lockout_dur = 0        
        self.__time = 0
    
    def __call__(self):
        return self.low if self.__is_dim else self.high

    def tick(self, delta_ms):
        self.__time += delta_ms
        
        if self.__is_dim:

            # Check if the dim duration has elapsed            
            if self.__time >= self.__dim_dur:
                self.__time -= self.__dim_dur

                self.__lockout_dur = int(random.uniform(self.lockout_min, self.lockout_max) * 1000)               
                self.__is_dim = False

        else:
           # Only attempt to flicker if not in lockout period
            if self.__time >= self.__lockout_dur:
                self.__time -= self.__lockout_dur
                
                self.__dim_dur = int(random.uniform(self.dim_min, self.dim_max) * 1000)
                self.__is_dim = True
