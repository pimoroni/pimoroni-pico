import time
from breakout_ioexpander import BreakoutIOExpander

ioe_adc_pin = 10

ioe = BreakoutIOExpander(0x18)

ioe.set_mode(ioe_adc_pin, BreakoutIOExpander.PIN_ADC)

while True:
    voltage = ioe.input_as_voltage(ioe_adc_pin)
    print("Voltage: ", voltage, sep="")

    time.sleep(0.02)
