import time, random
import picoexplorer as explorer

width = explorer.get_width()
height = explorer.get_height()

display_buffer = bytearray(width * height * 2)  # 2-bytes per pixel (RGB565)
explorer.init(display_buffer)

explorer.set_backlight(1.0)
explorer.set_audio_pin(0)

i = 1

while True:
    explorer.set_pen(120, 40, 60)    
    explorer.clear()
    
    adc0 = int(explorer.get_adc(0) * 120)
    adc1 = int(explorer.get_adc(1) * 120)
    adc2 = int(explorer.get_adc(2) * 120)
    
    explorer.set_pen(255, 255, 255)

    explorer.text("ADC0:", 20, 20, 100)    
    explorer.text("ADC1:", 20, 40, 100)    
    explorer.text("ADC2:", 20, 60, 100)    

    explorer.set_pen(adc0 * 2, 0, 0) 
    explorer.circle(90 + adc0, 26, 10)
    
    explorer.set_pen(0, adc1 * 2, 0) 
    explorer.circle(90 + adc1, 46, 10)

    explorer.set_pen(0, 0, adc2 * 2) 
    explorer.circle(90 + adc2, 66, 10)

    # example for the on-board A/B/X/Y buttons
    if explorer.is_pressed(explorer.BUTTON_A):
        explorer.set_pen(255, 255, 255)
        explorer.text("Button A pressed", 20, 110, 200)
    elif explorer.is_pressed(explorer.BUTTON_B):
        explorer.set_pen(255, 255, 255)
        explorer.text("Button B pressed", 20, 110, 200)
    elif explorer.is_pressed(explorer.BUTTON_X) and explorer.is_pressed(explorer.BUTTON_Y):
        explorer.set_pen(255, 255, 255)
        explorer.text("Buttons X and Y pressed", 20, 110, 200)
    elif explorer.is_pressed(explorer.BUTTON_X):
        explorer.set_pen(255, 255, 255)
        explorer.text("Button X pressed", 20, 110, 200)
    elif explorer.is_pressed(explorer.BUTTON_Y):
        explorer.set_pen(255, 255, 255)
        explorer.text("Button Y pressed", 20, 110, 200)
    else:
        # no button press was detected
        explorer.set_pen(255, 255, 255)
        explorer.text("Plug a jumper wire from GP0 to AUDIO to hear noise!", 20, 110, 200)    

    explorer.set_tone(i)

    if i > 600:        
        explorer.text("Motor 1: Forwards", 20, 180, 200)    
        explorer.set_motor(0, 0, 1)
    else:
        explorer.text("Motor 1: Backwards", 20, 180, 200)    
        explorer.set_motor(0, 1, 1)
    
    if i > 600:
        explorer.text("Motor 2: Forwards", 20, 200, 200)    
        explorer.set_motor(1, 0, 1)
    else:
        explorer.text("Motor 2: Backwards", 20, 200, 200)    
        explorer.set_motor(1, 1, 1)

    i = i + 20
    if i > 1000:
        i = 1
        
    explorer.update()
    time.sleep(0.01)
