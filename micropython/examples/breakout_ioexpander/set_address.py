"""
This example shows you how to change the I2C address of an IO Expander breakout (or any of our other Nuvoton based breakouts).
"""

from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS

# enter the current and desired I2C addresses here
EXISTING_ADDRESS = 0x18
NEW_ADDRESS = 0x20

i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)

# Uncomment these lines to change the address
# from breakout_ioexpander import BreakoutIOExpander
# ioe = BreakoutIOExpander(i2c, address=EXISTING_ADDRESS)
# ioe.set_address(NEW_ADDRESS)

# List all the connected I2C devices so we can see the change
print('Scanning i2c bus')
devices = i2c.scan()

if len(devices) == 0:
    print("No i2c device !")

else:
    print('i2c devices found:', len(devices))

for device in devices:
    print("Decimal address: ", device, " | Hexa address: ", hex(device))
