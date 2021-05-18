import time
from pimoroni_i2c import PimoroniI2C
from breakout_ioexpander import BreakoutIOExpander

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

ioe_adc_pin = 10

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
ioe = BreakoutIOExpander(i2c, address=0x18)

ioe.set_mode(ioe_adc_pin, BreakoutIOExpander.PIN_ADC)

while True:
    voltage = ioe.input_as_voltage(ioe_adc_pin)
    print("Voltage: ", voltage, sep="")

    time.sleep(0.02)
