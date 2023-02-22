"""
This example uses the Coinbase open API to collect the current exchange rates.
Use Switch A to change to a different base exchange currency.
"""

import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import time
import math
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY
import gc

URL = 'https://api.coinbase.com/v2/exchange-rates?currency={0}'

currencies = {"Bitcoin": "BTC", "Ethereun": "ETH", "Pound": "GBP", "Dollar": "USD", "Dogecoin": "DOGE"}
currency_keys = list(currencies.keys())

ref_currency_name = ""
currency_name = ""
currency_symbol = ""
currency_rate = ""
rate_keys = []

# diplay options
line_1_line = -2
line_2_line = 9
line_3_line = 20

ref_currency_index = 0

cycles_per_sequence = 120

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)


# for Handling the wifi connection
def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')


def get_data(currency_selected):

    graphics.set_pen(graphics.create_pen(20, 20, 20))
    graphics.clear()
    graphics.set_pen(graphics.create_pen(100, 100, 100))
    graphics.text("Get", 0, 10, scale=1, spacing=1)
    graphics.text("data", 8, 16, scale=1, spacing=1)
    cu.update(graphics)
    gc.collect()
    # open the json file
    print('Requesting URL:')
    print(URL.format(currencies[currency_selected]))
    r = urequests.get(URL.format(currencies[currency_selected]))
    gc.collect()
    # open the json data
    data_obj = r.json()
    print('Data obtained!')
    r.close()
    return data_obj


def calculate_xpos(length, cycle):
    cycle_phase = math.cos(math.pi * cycle / (cycles_per_sequence / 2))
    pos_x = int((-(length / 2) * 10) - (length / 2) * 10 * cycle_phase)
    return pos_x


def update_display(cycle):

    graphics.set_pen(graphics.create_pen(20, 20, 20))
    graphics.clear()
    graphics.set_pen(graphics.create_pen(100, 0, 0))
    graphics.text(ref_currency_name, calculate_xpos((len(ref_currency_name)), cycle), line_1_line, scale=2, spacing=1)
    graphics.set_pen(graphics.create_pen(100, 100, 0))
    if len(currency_symbol) > 3:
        graphics.text(currency_symbol, calculate_xpos((len(currency_symbol)), cycle), line_2_line, scale=2, spacing=1)
    else:
        graphics.text(currency_symbol, 0, line_2_line, scale=2, spacing=1)
    graphics.set_pen(graphics.create_pen(0, 100, 100))
    graphics.text(currency_rate, calculate_xpos((len(currency_rate)), cycle), line_3_line, scale=2, spacing=1)


def update_base_currency(index):
    fetched_data = 0
    global rates, rate_keys, currency_symbol, currency_rate, ref_currency_name
    fetched_data = get_data(currency_keys[index])
    rates = fetched_data['data']['rates']
    rate_keys = list(rates.keys())
    currency_symbol = rate_keys[index]
    currency_rate = str(rates[rate_keys[index]])
    ref_currency_name = "{0}-{1}".format(currency_keys[index], currencies[currency_keys[index]])
    gc.collect()


update_base_currency(ref_currency_index)
update_display(0)
cu.update(graphics)
cycle_count = 0
symbol_index = 0
print("Display {0} {1}".format(currency_symbol, currency_rate))

while 1:
    if cycle_count > 4 * cycles_per_sequence:
        cycle_count = 0
        symbol_index += 1
        if symbol_index > len(currency_keys):
            symbol_index = 0
            print("Display {0} {1}".format(currency_symbol, currency_rate))
        currency_symbol = rate_keys[symbol_index]
        currency_rate = rates[rate_keys[symbol_index]]

    if (cu.is_pressed(CosmicUnicorn.SWITCH_A)):
        ref_currency_index += 1
        if (ref_currency_index > len(currency_keys)):
            ref_currency_index = 0
        update_base_currency(ref_currency_index)

    if (cu.is_pressed(CosmicUnicorn.SWITCH_B)):
        cycle_count = 0
        symbol_index += 1

        if symbol_index > len(rate_keys):
            symbol_index = 0
        currency_symbol = rate_keys[symbol_index]
        currency_rate = rates[rate_keys[symbol_index]]

    update_display(cycle_count)
    cu.update(graphics)
    cycle_count += 1
    time.sleep(0.1)
