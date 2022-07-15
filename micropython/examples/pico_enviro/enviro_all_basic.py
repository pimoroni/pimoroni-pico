import time
from machine import Pin, ADC, UART
from pimoroni import RGBLED
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE
from pimoroni_i2c import PimoroniI2C
from breakout_ltr559 import BreakoutLTR559
from pms5003 import PMS5003

"""
This basic example shows how to read from all the sensors on Enviro+.
(plus the optional particulate sensor)
Prints results to the REPL.
"""

# change this to adjust temperature compensation
TEMPERATURE_OFFSET = 3

# set up the LED
led = RGBLED(6, 7, 10, invert=True)
led.set_rgb(255, 0, 0)

# set up the Pico's I2C
PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)

# set up BME688 and LTR559 sensors
bme = BreakoutBME68X(i2c, address=0x77)
ltr = BreakoutLTR559(i2c)

# setup analog channel for microphone
MIC_PIN = 26
mic = ADC(Pin(26))

# configure the PMS5003 for Enviro+
pms5003 = PMS5003(
    uart=UART(1, tx=Pin(8), rx=Pin(9), baudrate=9600),
    pin_enable=Pin(3),
    pin_reset=Pin(2),
    mode="active"
)

while True:
    # read BME688
    temperature, pressure, humidity, gas, status, _, _ = bme.read()
    heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"

    # correct temperature and humidity using an offset
    corrected_temperature = temperature - TEMPERATURE_OFFSET
    dewpoint = temperature - ((100 - humidity) / 5)
    corrected_humidity = 100 - (5 * (corrected_temperature - dewpoint))

    # read LTR559
    ltr_reading = ltr.get_reading()
    lux = ltr_reading[BreakoutLTR559.LUX]
    prox = ltr_reading[BreakoutLTR559.PROXIMITY]

    # read mic
    mic_reading = mic.read_u16()

    # read particle sensor
    particulate_reading = pms5003.read()

    if heater == "Stable" and ltr_reading is not None:
        led.set_rgb(0, 0, 0)
        print(f"""
             Temperature = {corrected_temperature} °C
             Humidity = {corrected_humidity} %
             Pressure = {pressure/100} hPa
             Gas = {gas}
             Lux = {lux}
             Mic = {mic_reading}
             Particulates (1.0) = {particulate_reading.pm_ug_per_m3(1.0)} ug/m3
             Particulates (2.5) = {particulate_reading.pm_ug_per_m3(2.5)} ug/m3
             Particulates (10) = {particulate_reading.pm_ug_per_m3(10)} ug/m3
             """)
    else:
        # light up the LED red if there's a problem with the BME688 or LTR559 sensor readings
        led.set_rgb(255, 0, 0)

    time.sleep(1.0)
