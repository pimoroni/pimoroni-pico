# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

import math
from picofx import Updatable

class TrafficLightFX(Updatable):
    def __init__(self, speed=1):
        self.speed = speed
        self.__states = [
            ((1, 0, 0), 5000),  # Red
            ((1, 1, 0), 2000),   # Red + Amber
            ((0, 0, 1), 10000),  # Green
            ((0, 1, 0), 2000)    # Amber
        ]
        self.__index = 0  # Start with Red state
        self.__time = 0  # Track time of last state change
        self.__state = self.__states[self.__index][0]
        self.__dur = self.__states[self.__index][1]
        self.__current = [0, 0, 0]

    def __call__(self, light):
        if light == "red":
            def fx():
                return self.__current[0]
            return self, fx

        if light == "amber":
            def fx():
                return self.__current[1]
            return self, fx
        
        if light == "green":
            def fx():
                return self.__current[2]
            return self, fx

        raise ValueError("light should be `red`, `amber`, or `green`")

    def tick(self, delta_ms):
        self.__time += int(delta_ms * self.speed)
        if self.__time >= self.__dur:
            self.__time -= self.__dur

            self.__index = (self.__index + 1) % len(self.__states)
            self.__state = self.__states[self.__index][0]
            self.__dur = self.__states[self.__index][1]
            
        for i in range(len(self.__current)):
            if self.__current[i] < self.__state[i]:
                self.__current[i] = min(self.__current[i] + delta_ms * self.speed * 0.002, self.__state[i])
            elif self.__current[i] > self.__state[i]:
                self.__current[i] = max(self.__current[i] - delta_ms * self.speed * 0.002, self.__state[i])
