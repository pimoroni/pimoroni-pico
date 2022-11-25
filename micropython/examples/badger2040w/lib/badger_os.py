"""Keep track of app state in persistent flash storage."""

import os
import gc
import time
import json
import machine
import badger2040w as badger2040


def get_battery_level():
    return 0
    # Battery measurement
    vbat_adc = machine.ADC(badger2040.PIN_BATTERY)
    vref_adc = machine.ADC(badger2040.PIN_1V2_REF)
    vref_en = machine.Pin(badger2040.PIN_VREF_POWER)
    vref_en.init(machine.Pin.OUT)
    vref_en.value(0)

    # Enable the onboard voltage reference
    vref_en.value(1)

    # Calculate the logic supply voltage, as will be lower that the usual 3.3V when running off low batteries
    vdd = 1.24 * (65535 / vref_adc.read_u16())
    vbat = (
        (vbat_adc.read_u16() / 65535) * 3 * vdd
    )  # 3 in this is a gain, not rounding of 3.3V

    # Disable the onboard voltage reference
    vref_en.value(0)

    # Convert the voltage to a level to display onscreen
    return vbat


def get_disk_usage():
    # f_bfree and f_bavail should be the same?
    # f_files, f_ffree, f_favail and f_flag are unsupported.
    f_bsize, f_frsize, f_blocks, f_bfree, _, _, _, _, _, f_namemax = os.statvfs("/")

    f_total_size = f_frsize * f_blocks
    f_total_free = f_bsize * f_bfree
    f_total_used = f_total_size - f_total_free

    f_used = 100 / f_total_size * f_total_used
    f_free = 100 / f_total_size * f_total_free

    return f_total_size, f_used, f_free


def state_running():
    state = {"running": "launcher"}
    state_load("launcher", state)
    return state["running"]


def state_clear_running():
    running = state_running()
    state_modify("launcher", {"running": "launcher"})
    return running != "launcher"


def state_set_running(app):
    state_modify("launcher", {"running": app})


def state_launch():
    app = state_running()
    if app is not None and app != "launcher":
        launch("_" + app)


def state_delete(app):
    try:
        os.remove("/state/{}.json".format(app))
    except OSError:
        pass


def state_save(app, data):
    try:
        with open("/state/{}.json".format(app), "w") as f:
            f.write(json.dumps(data))
            f.flush()
    except OSError:
        import os
        try:
            os.stat("/state")
        except OSError:
            os.mkdir("/state")
            state_save(app, data)


def state_modify(app, data):
    state = {}
    state_load(app, state)
    state.update(data)
    state_save(app, state)


def state_load(app, defaults):
    try:
        data = json.loads(open("/state/{}.json".format(app), "r").read())
        if type(data) is dict:
            defaults.update(data)
            return True
    except (OSError, ValueError):
        pass

    state_save(app, defaults)
    return False


def launch(file):
    state_set_running(file[1:])

    gc.collect()

    button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
    button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)

    def quit_to_launcher(pin):
        if button_a.value() and button_c.value():
            machine.reset()

    button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=quit_to_launcher)
    button_c.irq(trigger=machine.Pin.IRQ_RISING, handler=quit_to_launcher)

    try:
        try:
            __import__(file[1:])  # Try to import _[file] (drop underscore prefix)
        except ImportError:
            __import__(file)      # Failover to importing [_file]

    except ImportError:
        # If the app doesn't exist, notify the user
        warning(None, "Could not launch: " + file[1:])
        time.sleep(4.0)
    except Exception as e:
        # If the app throws an error, catch it and display!
        print(e)
        warning(None, str(e))
        time.sleep(4.0)

    # If the app exits or errors, do not relaunch!
    state_clear_running()
    machine.reset()  # Exit back to launcher


# Draw an overlay box with a given message within it
def warning(display, message, width=badger2040.WIDTH - 40, height=badger2040.HEIGHT - 40, line_spacing=20, text_size=0.6):
    print(message)

    if display is None:
        display = badger2040.Badger2040W()
        display.led(128)

    # Draw a light grey background
    display.set_pen(12)
    display.rectangle((badger2040.WIDTH - width) // 2, (badger2040.HEIGHT - height) // 2, width, height)

    # Take the provided message and split it up into
    # lines that fit within the specified width
    words = message.split(" ")

    lines = []
    current_line = ""
    for word in words:
        if display.measure_text(current_line + word + " ", text_size) < width:
            current_line += word + " "
        else:
            lines.append(current_line.strip())
            current_line = word + " "
    lines.append(current_line.strip())

    display.set_pen(0)

    # Display each line of text from the message, centre-aligned
    num_lines = len(lines)
    for i in range(num_lines):
        length = display.measure_text(lines[i], text_size)
        current_line = (i * line_spacing) - ((num_lines - 1) * line_spacing) // 2
        display.text(lines[i], (badger2040.WIDTH - length) // 2, (badger2040.HEIGHT // 2) + current_line, badger2040.WIDTH, text_size)

    display.update()
