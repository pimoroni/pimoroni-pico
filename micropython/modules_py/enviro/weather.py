import time
import math
from machine import Pin
from pimoroni import Analog

from enviro.base import EnviroBase
from breakout_bme280 import BreakoutBME280
from breakout_ltr559 import BreakoutLTR559


##################################################
# Enviro Weather
##################################################

MILES_PER_HOUR = 0
KILOMETRES_PER_HOUR = 1
METRES_PER_SECOND = 2


class EnviroWeather(EnviroBase):
    WIND_DIRECTION_PIN = 26
    WIND_SPEED_PIN = 9
    RAIN_PIN = 10

    WIND_DIRECTION_NORTH = 0
    WIND_DIRECTION_NORTHEAST = 45
    WIND_DIRECTION_EAST = 90
    WIND_DIRECTION_SOUTHEAST = 135
    WIND_DIRECTION_SOUTH = 180
    WIND_DIRECTION_SOUTHWEST = 225
    WIND_DIRECTION_WEST = 270
    WIND_DIRECTION_NORTHWEST = 315

    def __init__(self):
        EnviroBase.__init__(self)
        self.bme280 = BreakoutBME280(self.i2c, address=0x77)
        self.ltr559 = BreakoutLTR559(self.i2c)

        self.__wind_direction = Analog(self.WIND_DIRECTION_PIN)
        self.__wind_speed = Pin(self.WIND_SPEED_PIN, Pin.IN, Pin.PULL_UP)
        self.__rain = Pin(self.RAIN_PIN, Pin.IN, Pin.PULL_DOWN)

        if self.__rain.value():
            self.__woken_up_by = self.REASON_RAIN_TRIGGER

    def take_reading(self):
        raise NotImplementedError("This function is not yet implemented.")
        return self.bme280.read()

    def wind_direction(self):
        # adc reading voltage to cardinal direction taken from our python
        # library - each array index represents a 45 degree step around
        # the compass (index 0 == 0, 1 == 45, 2 == 90, etc.)
        # we find the closest matching value in the array and use the index
        # to determine the heading
        ADC_TO_DEGREES = (0.9, 2.0, 3.0, 2.8, 2.5, 1.5, 0.3, 0.6)

        value = self.__wind_direction.read_voltage()
        closest_index = -1
        closest_value = float('inf')

        for i in range(8):
            distance = abs(ADC_TO_DEGREES[i] - value)
            if distance < closest_value:
                closest_value = distance
                closest_index = i

        return closest_index * 45

    def cardinal_wind_direction(self):
        direction = self.wind_direction()
        if direction == self.WIND_DIRECTION_NORTH:
            return "N"
        if direction == self.WIND_DIRECTION_NORTHEAST:
            return "NE"
        if direction == self.WIND_DIRECTION_EAST:
            return "E"
        if direction == self.WIND_DIRECTION_SOUTHEAST:
            return "SE"
        if direction == self.WIND_DIRECTION_SOUTH:
            return "S"
        if direction == self.WIND_DIRECTION_SOUTHWEST:
            return "SW"
        if direction == self.WIND_DIRECTION_WEST:
            return "W"
        if direction == self.WIND_DIRECTION_NORTHWEST:
            return "NW"
        return ""

    def wind_speed(self, unit=KILOMETRES_PER_HOUR, sample_time_ms=1000):
        start = time.ticks_ms()

        was = self.__wind_speed.value()
        ticks = 0
        while time.ticks_ms() - start < sample_time_ms:
            now = self.__wind_speed.value()
            if not was and now:
                ticks += 1
            was = now

        radius = 7.0
        circumference = radius * 2.0 * math.pi
        factor = 0.0218  # scaling factor for wind speed in m/s
        tick_hz = (ticks * 1000.0) / (sample_time_ms * 2.0)  # two ticks per rotation

        # Calculate the wind speed in metres per second
        wind_m_s = tick_hz * circumference * factor

        if unit == MILES_PER_HOUR:
            return ((wind_m_s * 60 * 60) / 1000.0) * 0.621371

        if unit == KILOMETRES_PER_HOUR:
            return (wind_m_s * 60 * 60) / 1000.0

        return wind_m_s
