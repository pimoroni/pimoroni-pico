import time
from machine import Pin, PWM

from enviro.base import EnviroBase
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

    def __init__(self):
        EnviroBase.__init__(self)

        # Initialise our i2c sensors
        self.bme280 = BreakoutBME280(self.i2c, address=0x77)
        self.ltr559 = BreakoutLTR559(self.i2c)

        # Set up the sensor pins
        self.__sensors = []
        for i in range(NUM_SENSORS):
            self.__sensors.append(Pin(self.SENSOR_PINS[i], Pin.IN, Pin.PULL_DOWN))

        # Set up the pump pins
        self.__pumps = []
        for i in range(NUM_PUMPS):
            self.__pumps.append(Pin(self.PUMP_PINS[i], Pin.OUT))
            self.__pumps[i].value(False)

        # Set up the piezo buzzer
        self.piezo_pwm = PWM(Pin(self.PIEZO_PIN))

    def take_reading(self):
        raise NotImplementedError("This function is not yet implemented.")

    def sensor_ticks(self, sensor, sample_time_ms=100):
        if sensor < 0 or sensor > NUM_SENSORS:
            raise ValueError("sensor out of range. Expected SENSOR_A (0), SENSOR_B (1), or SENSOR_C (2)")

        start = time.ticks_ms()

        was = self.__sensors[sensor].value()
        ticks = 0
        while time.ticks_ms() - start < sample_time_ms:
            now = self.__sensors[sensor].value()
            if not was and now:
                ticks += 1
            was = now

        return ticks

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
