import time
import math
from machine import Pin
from pimoroni import Analog

from enviro.base import EnviroBase, CELCIUS, FAHRENHEIT, KELVIN, HECTOPASCAL, BAR, INHG, PSI
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

        # Initialise our i2c sensors
        self.bme280 = BreakoutBME280(self.i2c, address=0x77)
        self.ltr559 = BreakoutLTR559(self.i2c)

        # Set up the wind and rain sensors
        self.__wind_direction_pin = Analog(self.WIND_DIRECTION_PIN)
        self.__wind_speed_pin = Pin(self.WIND_SPEED_PIN, Pin.IN, Pin.PULL_UP)
        self.__rain_pin = Pin(self.RAIN_PIN, Pin.IN, Pin.PULL_DOWN)

        if self.__rain_pin.value():
            self.__woken_up_by = self.REASON_RAIN_TRIGGER

        self.has_reading = False

        self.__temperature_c = 0  # temperate in celcius
        self.__humidity_pct = 0  # humidty as a percentage
        self.__pressure_hpa = 0  # pressure in hectopascals
        self.__lux = 0
        self.__proximity = 0
        self.__wind_direction = 0
        self.__wind_speed = 0

    def take_reading(self):
        # get data from the bme280
        bme_data = self.bme280.read()
        self.__temperature_c = bme_data[0]
        self.__humidity_pct = bme_data[2]
        self.__pressure_hpa = bme_data[1] / 100.0

        # get data from the ltr559
        ltr_data = self.ltr559.get_reading()
        self.__lux = ltr_data[BreakoutLTR559.LUX]
        self.__proximity = ltr_data[BreakoutLTR559.PROXIMITY]

        self.__wind_speed = self.wind_speed()

        self.has_reading = True

        return True

    def readings(self):
        return {
            "temperature_celcius": self.temperature(),
            "humidity_percent": self.humidity(),
            "pressure_hectopascal": self.pressure(),
            "luminance_lux": self.__lux,
            "proximity": self.__proximity,
            "wind_direction": self.wind_direction(),
            "cardinal_wind_direction": self.cardinal_wind_direction(),
            "wind_speed": self.__wind_speed
        }

    def temperature(self, unit=CELCIUS):
        if unit == FAHRENHEIT:
            return round((self.__temperature_c * 9.0 / 5.0) + 32.0, 1)

        if unit == KELVIN:
            return round(self.__temperature_c + 273.15, 1)

        return round(self.__temperature_c, 1)

    def humidity(self):
        return round(self.__humidity_pct, 1)

    def pressure(self, unit=HECTOPASCAL):
        if unit == BAR:
            return round(self.__pressure_hpa / 1000.0, 2)

        if unit == INHG:
            return round(self.__pressure_hpa * 0.029529983071445, 1)

        if unit == PSI:
            return round(self.__pressure_hpa * 0.0145038, 1)

        return round(self.__pressure_hpa, 1)

    def wind_direction(self):
        # adc reading voltage to cardinal direction taken from our python
        # library - each array index represents a 45 degree step around
        # the compass (index 0 == 0, 1 == 45, 2 == 90, etc.)
        # we find the closest matching value in the array and use the index
        # to determine the heading
        ADC_TO_DEGREES = (0.9, 2.0, 3.0, 2.8, 2.5, 1.5, 0.3, 0.6)

        value = self.__wind_direction_pin.read_voltage()
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
        was = self.__wind_speed_pin.value()

        first_time = start
        while time.ticks_ms() - start < sample_time_ms:
            now = self.__wind_speed_pin.value()
            if not was and now:
                first_time = time.ticks_ms()
                break
            was = now

        was = now
        ticks = 0
        ticks_time = first_time
        while time.ticks_ms() - start < sample_time_ms:
            now = self.__wind_speed_pin.value()
            if not was and now:
                ticks += 1
                ticks_time = time.ticks_ms()
            was = now

        if ticks > 0:
            tick_hz = (1000 * ticks) / (float(ticks_time - first_time) * 2)  # two ticks per rotation
        else:
            tick_hz = 0.0

        radius = 7.0
        circumference = radius * 2.0 * math.pi
        factor = 0.0218  # scaling factor for wind speed in m/s

        # Calculate the wind speed in metres per second
        wind_m_s = tick_hz * circumference * factor

        if unit == MILES_PER_HOUR:
            return ((wind_m_s * 60 * 60) / 1000.0) * 0.621371

        if unit == KILOMETRES_PER_HOUR:
            return (wind_m_s * 60 * 60) / 1000.0

        return wind_m_s
