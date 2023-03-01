import gc
import time
from machine import reset
import inky_helper as ih

# Uncomment the line for your Inky Frame display size
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"

# Create a secrets.py with your Wifi details to be able to get the time
#
# secrets.py should contain:
# WIFI_SSID = "Your WiFi SSID"
# WIFI_PASSWORD = "Your WiFi password"

# A short delay to give USB chance to initialise
time.sleep(0.5)

# Setup for the display.
graphics = PicoGraphics(DISPLAY)
WIDTH, HEIGHT = graphics.get_bounds()
graphics.set_font("bitmap8")


def launcher():

    # Apply an offset for the Inky Frame 5.7".
    if HEIGHT == 448:
        y_offset = 20
    # Inky Frame 7.3"
    elif HEIGHT == 480:
        y_offset = 35
    # Inky Frame 4"
    else:
        y_offset = 0

    # Draws the menu
    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(0)

    graphics.set_pen(graphics.create_pen(255, 215, 0))
    graphics.rectangle(0, 0, WIDTH, 50)
    graphics.set_pen(0)
    title = "Launcher"
    title_len = graphics.measure_text(title, 4) // 2
    graphics.text(title, (WIDTH // 2 - title_len), 10, WIDTH, 4)

    graphics.set_pen(4)
    graphics.rectangle(30, HEIGHT - (340 + y_offset), WIDTH - 100, 50)
    graphics.set_pen(1)
    graphics.text("A. NASA Picture of the Day", 35, HEIGHT - (325 + y_offset), 600, 3)

    graphics.set_pen(6)
    graphics.rectangle(30, HEIGHT - (280 + y_offset), WIDTH - 150, 50)
    graphics.set_pen(1)
    graphics.text("B. Word Clock", 35, HEIGHT - (265 + y_offset), 600, 3)

    graphics.set_pen(2)
    graphics.rectangle(30, HEIGHT - (220 + y_offset), WIDTH - 200, 50)
    graphics.set_pen(1)
    graphics.text("C. Daily Activity", 35, HEIGHT - (205 + y_offset), 600, 3)

    graphics.set_pen(3)
    graphics.rectangle(30, HEIGHT - (160 + y_offset), WIDTH - 250, 50)
    graphics.set_pen(1)
    graphics.text("D. Headlines", 35, HEIGHT - (145 + y_offset), 600, 3)

    graphics.set_pen(0)
    graphics.rectangle(30, HEIGHT - (100 + y_offset), WIDTH - 300, 50)
    graphics.set_pen(1)
    graphics.text("E. Random Joke", 35, HEIGHT - (85 + y_offset), 600, 3)

    graphics.set_pen(graphics.create_pen(220, 220, 220))
    graphics.rectangle(WIDTH - 100, HEIGHT - (340 + y_offset), 70, 50)
    graphics.rectangle(WIDTH - 150, HEIGHT - (280 + y_offset), 120, 50)
    graphics.rectangle(WIDTH - 200, HEIGHT - (220 + y_offset), 170, 50)
    graphics.rectangle(WIDTH - 250, HEIGHT - (160 + y_offset), 220, 50)
    graphics.rectangle(WIDTH - 300, HEIGHT - (100 + y_offset), 270, 50)

    graphics.set_pen(0)
    note = "Hold A + E, then press Reset, to return to the Launcher"
    note_len = graphics.measure_text(note, 2) // 2
    graphics.text(note, (WIDTH // 2 - note_len), HEIGHT - 30, 600, 2)

    ih.led_warn.on()
    graphics.update()
    ih.led_warn.off()

    # Now we've drawn the menu to the screen, we wait here for the user to select an app.
    # Then once an app is selected, we set that as the current app and reset the device and load into it.

    # You can replace any of the included examples with one of your own,
    # just replace the name of the app in the line "ih.update_last_app("nasa_apod")"

    while True:
        if ih.inky_frame.button_a.read():
            ih.inky_frame.button_a.led_on()
            ih.update_state("nasa_apod")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_b.read():
            ih.inky_frame.button_b.led_on()
            ih.update_state("word_clock")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_c.read():
            ih.inky_frame.button_c.led_on()
            ih.update_state("daily_activity")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_d.read():
            ih.inky_frame.button_d.led_on()
            ih.update_state("news_headlines")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_e.read():
            ih.inky_frame.button_e.led_on()
            ih.update_state("random_joke")
            time.sleep(0.5)
            reset()


# Turn any LEDs off that may still be on from last run.
ih.clear_button_leds()
ih.led_warn.off()

if ih.inky_frame.button_a.read() and ih.inky_frame.button_e.read():
    launcher()

ih.clear_button_leds()

if ih.file_exists("state.json"):
    # Loads the JSON and launches the app
    ih.load_state()
    ih.launch_app(ih.state['run'])

    # Passes the the graphics object from the launcher to the app
    ih.app.graphics = graphics
    ih.app.WIDTH = WIDTH
    ih.app.HEIGHT = HEIGHT

else:
    launcher()

try:
    from secrets import WIFI_SSID, WIFI_PASSWORD
    ih.network_connect(WIFI_SSID, WIFI_PASSWORD)
except ImportError:
    print("Create secrets.py with your WiFi credentials")

# Get some memory back, we really need it!
gc.collect()

# The main loop executes the update and draw function from the imported app,
# and then goes to sleep ZzzzZZz

file = ih.file_exists("state.json")

print(file)

while True:
    ih.app.update()
    ih.led_warn.on()
    ih.app.draw()
    ih.led_warn.off()
    ih.sleep(ih.app.UPDATE_INTERVAL)
