from enviro.base import EnviroBase, CELCIUS, FAHRENHEIT, KELVIN, HECTOPASCAL, BAR, INHG, PSI
from breakout_bme68x import BreakoutBME68X
from breakout_bh1745 import BreakoutBH1745


##################################################
# Enviro Indoor
##################################################

class EnviroIndoor(EnviroBase):
    def __init__(self, take_first_reading=True):
        EnviroBase.__init__(self)
        self.bme688 = BreakoutBME68X(self.i2c, address=0x77)
        self.bh1745 = BreakoutBH1745(self.i2c)

        self.has_reading = False

        self.__temperature_c = 0  # temperate in celcius
        self.__humidity_pct = 0  # humidty as a percentage
        self.__pressure_hpa = 0  # pressure in hectopascals
        self.__gas_resistance_ohm = 0  # gas resistance reading in ohms
        self.__light = [0, 0, 0, 0]  # r, g, b, & c light sensor channels

        if take_first_reading:
            self.take_reading()

    def take_reading(self):
        # Read sensor data from bme688
        data = self.bme688.read()

        self.__temperature_c = data[0]
        self.__humidity_pct = data[2]
        self.__pressure_hpa = data[1] / 100.0
        self.__gas_resistance_ohm = data[3]

        # TODO: no success return flag from bh1745 driver, not consistent with bme688
        self.bh1745.measurement_time_ms(160)
        self.__light = self.bh1745.rgbc_raw()

        self.has_reading = True

        return True

    def temperature(self, unit=CELCIUS):
        if unit == FAHRENHEIT:
            return (self.__temperature_c * 9.0 / 5.0) + 32.0

        if unit == KELVIN:
            return self.__temperature_c + 273.15

        return self.__temperature_c

    def humidity(self):
        return self.__humidity_pct

    def pressure(self, unit=HECTOPASCAL):
        if unit == BAR:
            return self.__pressure_hpa / 1000.0

        if unit == INHG:
            return self.__pressure_hpa * 0.029529983071445

        if unit == PSI:
            return self.__pressure_hpa * 0.0145038

        return self.__pressure_hpa

    def light(self):
        return self.__light
