import time
import uasyncio
import WIFI_CONFIG
import inky_frame
from network_manager import NetworkManager
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"

# Set tz_offset to be the number of hours off of UTC for your local zone.
# Examples:  tz_offset = -7 # Pacific time (PST)
#            tz_offset =  1 # CEST (Paris)
tz_offset = 0
tz_seconds = tz_offset * 3600

# Sync the Inky (always on) RTC to the Pico W so that "time.localtime()" works.
inky_frame.pcf_to_pico_rtc()

# Avoid running code unless we've been triggered by an event
# Keeps this example from locking up Thonny when we want to tweak the code
if inky_frame.woken_by_rtc() or inky_frame.woken_by_button():
    graphics = PicoGraphics(DISPLAY)
    WIDTH, HEIGHT = graphics.get_bounds()

    graphics.set_pen(1)
    graphics.clear()

    # Look, just because this is an RTC demo,
    # doesn't mean we can't make it rainbow.
    for x in range(WIDTH):
        h = x / WIDTH
        p = graphics.create_pen_hsv(h, 1.0, 1.0)
        graphics.set_pen(p)
        graphics.line(x, 0, x, HEIGHT)

    graphics.set_pen(0)
    graphics.rectangle(0, 0, WIDTH, 14)
    graphics.set_pen(1)
    graphics.text("Inky Frame", 1, 0)
    graphics.set_pen(0)

    def status_handler(mode, status, ip):
        print(mode, status, ip)

    year, month, day, hour, minute, second, dow, _ = time.localtime(time.time() + tz_seconds)

    # Connect to the network and get the time if it's not set
    if year < 2023:
        connected = False
        network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler, client_timeout=60)

        t_start = time.time()
        try:
            uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
            connected = True
        except RuntimeError:
            pass
        t_end = time.time()

        if connected:
            inky_frame.set_time()

            graphics.text("Setting time from network...", 0, 40)
            graphics.text(f"Connection took: {t_end-t_start}s", 0, 60)
        else:
            graphics.text("Failed to connect!", 0, 40)

    # Display the date and time
    year, month, day, hour, minute, second, dow, _ = time.localtime(time.time() + tz_seconds)

    date_time = f"{year:04}/{month:02}/{day:02} {hour:02}:{minute:02}:{second:02}"

    graphics.set_font("bitmap8")

    text_scale = 8 if WIDTH == 800 else 6
    text_height = 8 * text_scale

    offset_left = (WIDTH - graphics.measure_text(date_time, scale=text_scale)) // 2
    offset_top = (HEIGHT - text_height) // 2

    graphics.set_pen(graphics.create_pen(50, 50, 50))
    graphics.text(date_time, offset_left + 2, offset_top + 2, scale=text_scale)
    graphics.set_pen(1)
    graphics.text(date_time, offset_left, offset_top, scale=text_scale)

    graphics.update()

    inky_frame.sleep_for(2)
