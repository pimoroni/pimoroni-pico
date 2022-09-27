import time
from pimoroni_i2c import PimoroniI2C
from breakout_ioexpander import BreakoutIOExpander
from adafruit_is31fl3731 import IS31FL3731
import sys
from machine import I2C, Pin

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

ENC_TERM_A   = 3
ENC_TERM_B   = 12

ENC_CHANNEL  = 1

#i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
i2c = I2C(0, sda=Pin(4), scl=Pin(5))
ioe = BreakoutIOExpander(i2c, address=0x18)

ioe.setup_rotary_encoder(ENC_CHANNEL, ENC_TERM_A, ENC_TERM_B, 6, count_microsteps=True)
#ioe.set_mode(ENC_TERM_A, BreakoutIOExpander.PIN_IN_PU)
#ioe.set_mode(ENC_TERM_B, BreakoutIOExpander.PIN_IN_PU)

last_count = -1

last_enc_a = False
last_enc_b = False

import sys
from machine import Pin
p15 = Pin(15, Pin.OUT)
p15.value(True)

last_sp = 0

while True:
    count = ioe.read_rotary_encoder(ENC_CHANNEL) // 2    
    if count != last_count:
        if count - last_count > 0:
            print("Clockwise, Count = ", count)
        else:
            print("Counter Clockwise, Count = ", count)
            
        if count == 0 and (last_count > 1 or last_count < -1):
            p15.value(False)
            #sys.exit()
            
        last_count = count
        
    '''
    enc_a = bool(ioe.input(ENC_TERM_A))
    enc_b = bool(ioe.input(ENC_TERM_B))
    
    if enc_a is not last_enc_a:
        if enc_a:
            print("ENC A high")
        else:
            print("ENC A low")
        last_enc_a = enc_a
    
    if enc_b is not last_enc_b:
        if enc_b:
            print("ENC B high")
        else:
            print("ENC B low")
        last_enc_b = enc_b
    '''    

    #time.sleep(0.001)