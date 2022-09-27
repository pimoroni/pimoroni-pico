import time
from pimoroni_i2c import PimoroniI2C
from breakout_ioexpander import BreakoutIOExpander
from adafruit_is31fl3731 import IS31FL3731
import sys
from machine import I2C, Pin

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

s1_pin = 1
s2_pin = 13
s3_pin = 11
s4_pin = 4
s5_pin = 2

ENC_TERM_A   = 3
ENC_TERM_B   = 12

ENC_CHANNEL  = 1

#i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
i2c = I2C(0, sda=Pin(4), scl=Pin(5))
ioe = BreakoutIOExpander(i2c, address=0x18)#, interrupt=3)
ioe.enable_interrupt_out(pin_swap=True)
#ioe.set_address(0x13)

#2c.writeto(0x18, bytearray([0xF9, 0x06]))
#i2c.writeto(0x18, bytearray([0x00, 0b11000000]))
#i2c.writeto(0x18, bytearray([0x01, 0b00110001]))

ioe.set_mode(s1_pin, BreakoutIOExpander.PIN_IN_PU)
ioe.set_mode(s2_pin, BreakoutIOExpander.PIN_IN_PU)
ioe.set_mode(s3_pin, BreakoutIOExpander.PIN_IN_PU)
ioe.set_mode(s4_pin, BreakoutIOExpander.PIN_IN_PU)
ioe.set_mode(s5_pin, BreakoutIOExpander.PIN_IN_PU)

ioe.setup_rotary_encoder(ENC_CHANNEL, ENC_TERM_A, ENC_TERM_B, 6, count_microsteps=True)
#ioe.set_mode(ENC_TERM_A, BreakoutIOExpander.PIN_IN_PU)
#ioe.set_mode(ENC_TERM_B, BreakoutIOExpander.PIN_IN_PU)
ioe.set_pin_interrupt(s1_pin, True)
ioe.set_pin_interrupt(s2_pin, True)
ioe.set_pin_interrupt(s3_pin, True)
ioe.set_pin_interrupt(s4_pin, True)
ioe.set_pin_interrupt(s5_pin, True)

display = IS31FL3731(i2c, address=0x77)
display2 = IS31FL3731(i2c, address=0x77-3)
display.fill(10)
display2.fill(10)

time.sleep(1)

mapping = ((128, 32, 48),
           (129, 33, 49),
           (130, 17, 50),
           (131, 18, 34),
           (132, 19, 35),
           (133, 20, 36),
           (134, 21, 37),
           (112, 80, 96),
           (113, 81, 97),
           (114, 82, 98),
           (115, 83, 99),
           (116, 84, 100),
           (117, 68, 101),
           (118, 69, 85),
           (127, 47, 63),
           (121, 41, 57),
           (122, 25, 58),
           (123, 26, 42),
           (124, 27, 43),
           (125, 28, 44),
           (126, 29, 45),
           (15, 95, 111),
           (8, 89, 105),
           (9, 90, 106))


def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q

'''
while True:
    for x in range(0, 24):
        single = mapping[x]
        display.pixel(single[0], 0, 255)
        display.pixel(single[1], 0, 255)
        display.pixel(single[2], 0, 255)
        print(x)
        time.sleep(0.05)
        display.pixel(single[0], 0, 0)
        display.pixel(single[1], 0, 0)
        display.pixel(single[2], 0, 0)
'''

last_s1 = True
last_s2 = True
last_s3 = True
last_s4 = True
last_s5 = True
last_count = -1

last_enc_a = False
last_enc_b = False

toggler = False

import sys
from machine import Pin
p15 = Pin(15, Pin.OUT)
p15.value(True)

last_sp = 0

while True:
    s1 = bool(ioe.input(s1_pin))
    s2 = bool(ioe.input(s2_pin))
    s3 = bool(ioe.input(s3_pin))
    s4 = bool(ioe.input(s4_pin))
    s5 = bool(ioe.input(s5_pin))
    if s1 is not last_s1:
        if s1:
            print("Centre (S1) has been released")
        else:
            print("Centre (S1) has been pressed")
        last_s1 = s1
        
    if s2 is not last_s2:
        if s2:
            print("Up (S2) has been released")
        else:
            print("Up (S2) has been pressed")
        last_s2 = s2

    if s3 is not last_s3:
        if s3:
            print("Left (S3) has been released")
        else:
            print("Left (S3) has been pressed")
        last_s3 = s3

    if s4 is not last_s4:
        if s4:
            print("Down (S4) has been released")
        else:
            print("Down (S4) has been pressed")
        last_s4 = s4
        
    if s5 is not last_s5:
        if s5:
            print("Right (S5) has been released")
        else:
            print("Right (S5) has been pressed")
        last_s5 = s5
       
    if ioe.get_interrupt_flag():
        ioe.clear_interrupt_flag()
        
    count = ioe.read_rotary_encoder(ENC_CHANNEL) // 2    
    if count != last_count:
        if count - last_count > 0:
            print("Clockwise, Count = ", count)
        else:
            print("Counter Clockwise, Count = ", count)
            
        if count == 0 and (last_count > 1 or last_count < -1):
            p15.value(False)
            #sys.exit()
            
        last_single = mapping[last_count % 24]
        display.pixel(last_single[0], 0, 0)
        display.pixel(last_single[1], 0, 0)
        display.pixel(last_single[2], 0, 0)
        display2.pixel(last_single[0], 0, 0)
        display2.pixel(last_single[1], 0, 0)
        display2.pixel(last_single[2], 0, 0)
        single = mapping[count % 24]
        r, g, b = hsv_to_rgb(count / 24, 1.0, 1.0)
        display.pixel(single[0], 0, int(255 * r))
        display.pixel(single[1], 0, int(255 * g))
        display.pixel(single[2], 0, int(255 * b))
        display2.pixel(single[0], 0, int(255 * r))
        display2.pixel(single[1], 0, int(255 * g))
        display2.pixel(single[2], 0, int(255 * b))
        last_count = count
    
            
    i2c.writeto(0x13, bytearray([0x41]))
    sp = i2c.readfrom(0x13, 1)
    if sp != last_sp:
        print("SP =", sp)
    last_sp = sp
       
    
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