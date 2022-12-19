from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
import gc
import time
from machine import reset
import inky_helper as ih

# Create a secrets.py with your Wifi details to be able to get the time
#
# secrets.py should contain:
# WIFI_SSID = "Your WiFi SSID"
# WIFI_PASSWORD = "Your WiFi password"

# Setup for the display.
graphics = PicoGraphics(DISPLAY)
WIDTH, HEIGHT = graphics.get_bounds()
graphics.set_font("bitmap8")


def launcher():
    # Draws the menu
    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(0)

    graphics.text("Inky Launcher", 5, 5, 350, 5)
    graphics.text("Menu", 5, 50, 350, 3)

    graphics.text("A. NASA Picture Of the Day", 5, 90, 350, 3)
    graphics.text("B. Word Clock", 5, 130, 350, 3)
    graphics.text("C. Daily Activity", 5, 170, 350, 3)
    graphics.text("D. Headlines", 5, 210, 350, 3)
    graphics.text("E. Random Joke", 5, 250, 350, 3)

    graphics.update()

    # Now we've drawn the menu to the screen, we wait here for the user to select an app.
    # Then once an app is selected, we set that as the current app and reset the device and load into it.

    # You can replace any of the included examples with one of your own,
    # just replace the name of the app in the line "ih.update_last_app("nasa_apod")"

    while True:
        if ih.inky_frame.button_a.read():
            ih.button_a_led.on()
            ih.update_state("nasa_apod")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_b.read():
            ih.button_b_led.on()
            ih.update_state("word_clock")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_c.read():
            ih.button_c_led.on()
            ih.update_state("daily_activity")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_d.read():
            ih.button_d_led.on()
            ih.update_state("news_headlines")
            time.sleep(0.5)
            reset()
        if ih.inky_frame.button_e.read():
            ih.button_e_led.on()
            ih.update_state("random_joke")
            time.sleep(0.5)
            reset()


# Turn any LEDs off that may still be on from last run.
ih.clear_button_leds()
ih.led_warn.off()

if ih.pressed() == ih.inky_frame.button_e:
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
    ih.app.draw()
    ih.sleep(ih.app.UPDATE_INTERVAL)
