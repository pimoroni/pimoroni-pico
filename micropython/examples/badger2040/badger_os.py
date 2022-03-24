"""Keep track of app state in persistent flash storage."""

import os
import gc
import machine
import badger2040

STATE_FILE = "appstate.txt"


def get_battery_level():
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


def state_app():
    try:
        with open(STATE_FILE, "r") as f:
            return f.readline().strip()
    except OSError:
        return None


def state_launch():
    app = state_app()
    if app is not None:
        launch("_" + app)


def state_delete():
    try:
        os.remove(STATE_FILE)
    except OSError:
        pass


def state_save(title, *args):
    with open(STATE_FILE, "w") as f:
        f.write("{}\n".format(title))
        for arg in args:
            f.write("{}\n".format(arg))


def state_load(title, *defaults):
    data = []
    try:
        with open(STATE_FILE, "r") as f:
            if f.readline().strip() != title:
                return defaults
            for default in defaults:
                t = type(default)
                if t is bool:
                    data.append(f.readline().strip() == "True")
                else:
                    data.append(t(f.readline().strip()))
            return data
    except OSError:
        return defaults


def launch(file):
    for k in locals().keys():
        if k not in ("gc", "file", "machine"):
            del locals()[k]
    gc.collect()
    try:
        __import__(file[1:])  # Try to import _[file] (drop underscore prefix)
    except ImportError:
        __import__(file)  # Failover to importing [_file]
    machine.reset()  # Exit back to launcher
