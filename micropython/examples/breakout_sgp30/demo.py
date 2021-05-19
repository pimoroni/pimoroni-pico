import time
from pimoroni_i2c import PimoroniI2C
from breakout_sgp30 import BreakoutSGP30

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}  # i2c pins 4, 5 for Breakout Garden
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}  # Default i2c pins for Pico Explorer

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
sgp30 = BreakoutSGP30(i2c)

print("SGP30 initialised - about to start measuring without waiting")

sgp30.start_measurement(False)
id = sgp30.get_unique_id()
print("Started measuring for id 0x", '{:04x}'.format(id[0]), '{:04x}'.format(id[1]), '{:04x}'.format(id[2]), sep="")

j = 0
while True:
    j += 1
    air_quality = sgp30.get_air_quality()
    eCO2 = air_quality[BreakoutSGP30.ECO2]
    TVOC = air_quality[BreakoutSGP30.TVOC]

    air_quality_raw = sgp30.get_air_quality_raw()
    H2 = air_quality_raw[BreakoutSGP30.H2]
    ETHANOL = air_quality_raw[BreakoutSGP30.ETHANOL]

    print(j, ": CO2 ", eCO2, " TVOC ", TVOC, ", raw ", H2, " ", ETHANOL, sep="")
    if j == 30:
        print("Resetting device")
        sgp30.soft_reset()
        time.sleep(0.5)
        print("Restarting measurement, waiting 15 secs before returning")
        sgp30.start_measurement(True)
        print("Measurement restarted, now read every second")

    time.sleep(1.0)
