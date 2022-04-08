import time

from breakout_bme68x import BreakoutBME68X, OVERSAMPLING_4X
from pimoroni_i2c import PimoroniI2C
import badger2040

badger2040.system_speed(badger2040.SYSTEM_SLOW)

i2c = PimoroniI2C(sda=4, scl=5)
bme = BreakoutBME68X(i2c)
bme.configure(os_humidity=OVERSAMPLING_4X)

display = badger2040.Badger2040()
display.led(128)
display.update_speed(badger2040.UPDATE_FAST)

HIST_LEN = 288
temp_hist = [[-1] * HIST_LEN, [-1] * HIST_LEN, [-1] * HIST_LEN]
pres_hist = [[-1] * HIST_LEN, [-1] * HIST_LEN, [-1] * HIST_LEN]
humid_hist = [[-1] * HIST_LEN, [-1] * HIST_LEN, [-1] * HIST_LEN]
hist_idx = [0] * 3

DISPLAY_TEXT = 0
DISPLAY_TEMP = 1
DISPLAY_PRES = 2
DISPLAY_HUMID = 3
DISPLAY_MAX = 3
display_screen = 0
display_hist = 0

X_OFFSET = 296 - HIST_LEN

while True:
    temp, pres, humidity = bme.read_tph()
    pres /= 100
    temp_hist[0][hist_idx[0]], pres_hist[0][hist_idx[0]], humid_hist[0][hist_idx[0]] = temp, pres, humidity
    
    hist_idx[0] += 1
    if (hist_idx[0] % 12) == 0:
        temp_hist[1][hist_idx[1]] = sum(temp_hist[0][hist_idx[0]-12:hist_idx[0]]) / 12
        pres_hist[1][hist_idx[1]] = sum(pres_hist[0][hist_idx[0]-12:hist_idx[0]]) / 12
        humid_hist[1][hist_idx[1]] = sum(humid_hist[0][hist_idx[0]-12:hist_idx[0]]) / 12
        hist_idx[1] += 1
        if (hist_idx[1] % 12) == 0:
            temp_hist[2][hist_idx[2]] = sum(temp_hist[1][hist_idx[1]-12:hist_idx[1]]) / 12
            pres_hist[2][hist_idx[2]] = sum(pres_hist[1][hist_idx[1]-12:hist_idx[1]]) / 12
            humid_hist[2][hist_idx[2]] = sum(humid_hist[1][hist_idx[1]-12:hist_idx[1]]) / 12
            hist_idx[2] += 1
            if hist_idx[2] >= HIST_LEN: hist_idx[2] = 0
        if hist_idx[1] >= HIST_LEN: hist_idx[1] = 0
    if hist_idx[0] >= HIST_LEN: hist_idx[0] = 0
    
    update = (hist_idx[0] % 12) == 1
    if display_screen != DISPLAY_TEXT and display_hist == 2:
        update = update and (hist_idx[1] % 12) == 1
    
    if display.pressed(badger2040.BUTTON_A):
        display_screen -= 1
        if display_screen < 0: display_screen = DISPLAY_MAX
        update = True
    if display.pressed(badger2040.BUTTON_C):
        display_screen += 1
        if display_screen > DISPLAY_MAX: display_screen = 0
        update = True
    if display.pressed(badger2040.BUTTON_B) and display_screen != DISPLAY_TEXT:
        display_hist += 1
        if display_hist > 2: display_hist = 0
        update = True

    if update:
        if display_screen == DISPLAY_TEXT:
            display.pen(15)
            display.clear()
            display.pen(0)
            
            display.font("sans")
            display.text("Temp: {:.2f}C".format(temp), 5, 20, 1)
            display.text("Press: {:.1f}mB".format(pres), 5, 60, 1)
            display.text("Humid: {:.1f}%".format(humidity), 5, 100, 1)
            display.update()
        else:
            if display_screen == DISPLAY_TEMP:
                hist_head = "Temperature (C)"
                hist_values = temp_hist[display_hist]
            elif display_screen == DISPLAY_PRES:
                hist_head = "Pressure (mB)"
                hist_values = pres_hist[display_hist]
            elif display_screen == DISPLAY_HUMID:
                hist_head = "Humidity (%)"
                hist_values = humid_hist[display_hist]
                
            if display_hist == 0:
                hist_head = "5 min - " + hist_head
            elif display_hist == 1:
                hist_head = "1 hour - " + hist_head
            elif display_hist == 2:
                hist_head = "12 hour - " + hist_head

            hist_min = 10000
            hist_max = 0
            for i in range(HIST_LEN):
                if hist_values[i] >= 0 and hist_values[i] < hist_min: hist_min = hist_values[i]
                if hist_values[i] > hist_max: hist_max = hist_values[i]
                
            if hist_max - hist_min < 1:
                extra = (1 - (hist_max - hist_min)) / 2
                hist_max += extra
                hist_min -= extra

            display.pen(15)
            display.clear()
            display.pen(0)
            
            display.font("bitmap8")
            display.text(hist_head, 148 - display.measure_text(hist_head, 1) // 2, 0, 1)
            
            if hist_min > 2000:
                display.font("sans")
                display.text("No data", 148 - display.measure_text("No data", 1) // 2, 64, 1)
            else:
                display.text("{:.1f}".format(hist_max), 0, 10, 1)
                display.text("{:.1f}".format(hist_min), 0, 120, 1)
                
                for i in range(HIST_LEN):
                    val = hist_values[i]
                    if val >= hist_min and val <= hist_max:
                        y = (val - hist_min) / (hist_max - hist_min)
                        y = 127 - 117 * y
                        display.pixel(i + X_OFFSET, int(y))
                        
                display.line(hist_idx[display_hist] + X_OFFSET, 10, hist_idx[display_hist] + X_OFFSET, 127)
                val = hist_values[hist_idx[display_hist] - 1]
                y = (val - hist_min) / (hist_max - hist_min)
                y = 127 - 117 * y
                val_text = "{:.1f}".format(hist_values[hist_idx[display_hist] - 1])
                display.text(val_text, max(0, hist_idx[display_hist] + X_OFFSET - display.measure_text(val_text, 1)), int(y) - 10, 1)
            
            display.update()
    else:
        time.sleep(1)