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

        # needed to fix the readings from the sensor
        self.i2c.writeto_mem(0x38, 0x44, b'\x02')

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

    def readings(self):
        return {
            "temperature_celcius": self.temperature(),
            "humidity_percent": self.humidity(),
            "pressure_hectopascal": self.pressure(),
            "luminance_lux": self.luminance(),
            "color_temperature_kelvin": self.color_temperature()
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

    def light(self):
        return self.__light

    def color_temperature(self):
        r, g, b, c = self.__light

        if (g < 1) or (r + g + b < 1):
            return 0

        r_ratio = r / (r + g + b)
        b_ratio = b / (r + g + b)
        e = 2.71828

        ct = 0

        if c / g < 0.160:
            b_eff = min(b_ratio * 3.13, 1)
            ct = ((1 - b_eff) * 12746 * (e ** (-2.911 * r_ratio))) + (b_eff * 1637 * (e ** (4.865 * b_ratio)))
        else:
            b_eff = min(b_ratio * 10.67, 1)
            ct = ((1 - b_eff) * 16234 * (e ** (-2.781 * r_ratio))) + (b_eff * 1882 * (e ** (4.448 * b_ratio)))

        if ct > 10000:
            ct = 10000

        return round(ct)

    def luminance(self):
        r, g, b, c = self.__light
        if g < 1:
            tmp = 0
        elif (c / g < 0.160):
            tmp = 0.202 * r + 0.766 * g
        else:
            tmp = 0.159 * r + 0.646 * g

        tmp = 0 if tmp < 0 else tmp

        integration_time = 160
        gain = 1

        return round(tmp / gain / integration_time * 160)
