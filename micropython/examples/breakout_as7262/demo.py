import time
from breakout_as7262 import BreakoutAS7262

as7262 = BreakoutAS7262()

dev_type = as7262.device_type();
hw_version = as7262.hardware_version();
fw_version = as7262.firmware_version();

print("Device: ", dev_type, "HW: ", hw_version, sep="", end=", ")
print("FW: ", fw_version[0], ".", fw_version[1], ".", fw_version[2])

as7262.set_gain(BreakoutAS7262.X16)
as7262.set_measurement_mode(BreakoutAS7262.CONT_ROYGBR)
as7262.set_illumination_current(BreakoutAS7262.MA12)
as7262.set_indicator_current(BreakoutAS7262.MA4)
#as7262.set_leds(False, False)

while True:
    reading = as7262.read()
    print("R:", reading[0], end=" ")
    print("O:", reading[1], end=" ")
    print("Y:", reading[2], end=" ")
    print("G:", reading[3], end=" ")
    print("B:", reading[4], end=" ")
    print("V:", reading[5])
    
    time.sleep(1.0)
