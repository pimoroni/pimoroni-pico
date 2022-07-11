import rp2
import network
import machine
import uasyncio


class NetworkManager:
    _ifname = ("Client", "Access Point")

    def __init__(self, country="GB", client_timeout=30, access_point_timeout=5, status_handler=None):
        rp2.country(country)
        self._ap_if = network.WLAN(network.AP_IF)
        self._sta_if = network.WLAN(network.STA_IF)

        self._mode = network.STA_IF
        self._client_timeout = client_timeout
        self._access_point_timeout = access_point_timeout
        self._status_handler = status_handler
        self.UID = ("{:02X}" * 8).format(*machine.unique_id())

    def isconnected(self):
        return self._sta_if.isconnected() or self._ap_if.isconnected()

    def ifaddress(self):
        if self._sta_if.isconnected():
            return self._sta_if.ifconfig()[0]
        if self._ap_if.isconnected():
            return self._ap_if.ifconfig()[0]
        return '0.0.0.0'

    def disconnect(self):
        if self._sta_if.isconnected():
            self._sta_if.disconnect()
        if self._ap_if.isconnected():
            self._ap_if.disconnect()

    async def wait(self, mode):
        while not self.isconnected():
            self._handle_status(mode, None)
            await uasyncio.sleep_ms(1000)

    def _handle_status(self, mode, status):
        if self._status_handler is not None:
            self._status_handler(self._ifname[mode], status, self.ifaddress())

    async def client(self, ssid, psk):
        if self._sta_if.isconnected():
            return

        self._ap_if.disconnect()
        self._ap_if.active(False)
        self._ap_if.deinit()

        self._sta_if = network.WLAN(network.STA_IF)
        self._sta_if.active(True)
        self._sta_if.connect(ssid, psk)

        try:
            await uasyncio.wait_for(self.wait(network.STA_IF), self._client_timeout)
            self._handle_status(network.STA_IF, True)

        except uasyncio.TimeoutError:
            self._sta_if.active(False)
            self._handle_status(network.STA_IF, False)
            raise RuntimeError("WIFI Client Failed")

    async def access_point(self):
        if self._ap_if.isconnected():
            return

        self._sta_if.disconnect()
        self._sta_if.active(False)
        self._sta_if.deinit()

        self._ap_if = network.WLAN(network.AP_IF)
        self._ap_if.active(True)

        try:
            await uasyncio.wait_for(self.wait(network.AP_IF), self._access_point_timeout)
            self._handle_status(network.AP_IF, True)

        except uasyncio.TimeoutError:
            self._sta_if.active(False)
            self._handle_status(network.AP_IF, False)
            raise RuntimeError("WIFI AP Failed")