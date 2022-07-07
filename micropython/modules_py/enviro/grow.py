import time
from machine import Pin, PWM

from enviro.base import EnviroBase, CELCIUS, FAHRENHEIT, KELVIN, HECTOPASCAL, BAR, INHG, PSI
from breakout_bme280 import BreakoutBME280
from breakout_ltr559 import BreakoutLTR559


##################################################
# Enviro Grow
##################################################

# Index Constants
SENSOR_A = 0
SENSOR_B = 1
SENSOR_C = 2

PUMP_A = 0
PUMP_B = 1
PUMP_C = 2

# Count Constants
NUM_SENSORS = 3
NUM_PUMPS = 3


class EnviroGrow(EnviroBase):
    SENSOR_PINS = (15, 14, 13)
    PUMP_PINS = (12, 11, 10)
    PIEZO_PIN = 28

    DEFAULT_FREQUENCY = 1000

    def __init__(self, take_first_reading=True):
        EnviroBase.__init__(self)

        # Initialise our i2c sensors
        self.bme280 = BreakoutBME280(self.i2c, address=0x77)
        self.ltr559 = BreakoutLTR559(self.i2c)

        # Set up the sensor pins
        self.__sensors = []
        for i in range(NUM_SENSORS):
            self.__sensors.append(Pin(self.SENSOR_PINS[i], Pin.IN, Pin.PULL_DOWN))

        self.has_reading = False

        self.__temperature_c = 0  # temperate in celcius
        self.__humidity_pct = 0  # humidty as a percentage
        self.__pressure_hpa = 0  # pressure in hectopascals
        self.__lux = 0
        self.__proximity = 0
        self.__moisture = [0, 0, 0]

        # Set up the pump pins
        self.__pumps = []
        for i in range(NUM_PUMPS):
            self.__pumps.append(Pin(self.PUMP_PINS[i], Pin.OUT))
            self.__pumps[i].value(False)

        # Set up the piezo buzzer
        self.piezo_pwm = PWM(Pin(self.PIEZO_PIN))

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

        # get readings from the moisture sensors
        self.__moisture[SENSOR_A] = self.sensor_ticks(SENSOR_A)
        self.__moisture[SENSOR_B] = self.sensor_ticks(SENSOR_B)
        self.__moisture[SENSOR_C] = self.sensor_ticks(SENSOR_C)

        self.has_reading = True

        return True

    def readings(self):
        return {
            "temperature_celcius": self.temperature(),
            "humidity_percent": self.humidity(),
            "pressure_hectopascal": self.pressure(),
            "luminance_lux": self.__lux,
            "proximity": self.__proximity,
            "moisture_a": self.__moisture[SENSOR_A],
            "moisture_b": self.__moisture[SENSOR_B],
            "moisture_c": self.__moisture[SENSOR_C]
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

    def sensor_ticks(self, sensor, sample_time_ms=2000):
        if sensor < 0 or sensor > NUM_SENSORS:
            raise ValueError("sensor out of range. Expected SENSOR_A (0), SENSOR_B (1), or SENSOR_C (2)")

        start = time.ticks_ms()
        was = self.__sensors[sensor].value()

        first_time = start
        while time.ticks_ms() - start < sample_time_ms:
            now = self.__sensors[sensor].value()
            if not was and now:
                first_time = time.ticks_ms()
                break
            was = now

        was = now
        ticks = 0
        tick_time = first_time
        while time.ticks_ms() - start < sample_time_ms:
            now = self.__sensors[sensor].value()
            if not was and now:
                ticks += 1
                tick_time = time.ticks_ms()
            was = now

        if ticks > 0:
            return float(tick_time - first_time) / float(ticks)
        else:
            return -1

    def pump(self, pump, on):
        if pump < 0 or pump > NUM_PUMPS:
            raise ValueError("pump out of range. Expected PUMP_A (0), PUMP_B (1), or PUMP_C (2)")
        # Only allow one pump to be active at a time, for current reasons
        for i in range(NUM_PUMPS):
            if i is not pump:
                self.__pumps[i].value(False)
        self.__pumps[pump].value(on)

    def play_tone(self, frequency):
        try:
            self.piezo_pwm.freq(frequency)
        except ValueError:
            self.stop_playing()
            raise ValueError("frequency of range. Expected greater than 0")

        self.piezo_pwm.duty_u16(32768)

    def stop_playing(self):
        self.piezo_pwm.duty_u16(0)
