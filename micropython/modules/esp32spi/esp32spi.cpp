#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

//#include "../../../pimoroni-pico/libraries/pico_scroll/pico_scroll.hpp"

//using namespace pimoroni;

class ESP32SPI {

public:
    void init(void) {

    }
};

ESP32SPI *esp = nullptr;


extern "C" {
#include "esp32spi.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as esp32spi is not initialised. Call esp32spi.init() first."

mp_obj_t esp32spi_init() {
    if(esp == nullptr)
        esp = new ESP32SPI();
    esp->init();

    // self._debug = debug
    // self.set_psk = False
    // self.set_crt = False
    // self._buffer = bytearray(10)
    // self._pbuf = bytearray(1)  # buffer for param read
    // self._sendbuf = bytearray(256)  # buffer for command sending
    // self._socknum_ll = [[0]]  # pre-made list of list of socket #

    // self._spi_device = SPIDevice(spi, cs_pin, baudrate=8000000)
    // self._cs = cs_pin
    // self._ready = ready_pin
    // self._reset = reset_pin
    // self._gpio0 = gpio0_pin
    // self._cs.direction = Direction.OUTPUT
    // self._ready.direction = Direction.INPUT
    // self._reset.direction = Direction.OUTPUT
    // # Only one TLS socket at a time is supported so track when we already have one.
    // self._tls_socket = None
    // if self._gpio0:
    //     self._gpio0.direction = Direction.INPUT
    // self.reset()
    return mp_const_none;
}

mp_obj_t esp32spi_reset() {
    // """Hard reset the ESP32 using the reset pin"""
    // if self._debug:
    //     print("Reset ESP32")
    // if self._gpio0:
    //     self._gpio0.direction = Direction.OUTPUT
    //     self._gpio0.value = True  # not bootload mode
    // self._cs.value = True
    // self._reset.value = False
    // time.sleep(0.01)  # reset
    // self._reset.value = True
    // time.sleep(0.75)  # wait for it to boot up
    // if self._gpio0:
    //     self._gpio0.direction = Direction.INPUT
    return mp_const_none;
}

mp_obj_t esp32spi_status() {
    // """The status of the ESP32 WiFi core. Can be WL_NO_SHIELD or WL_NO_MODULE
    // (not found), WL_IDLE_STATUS, WL_NO_SSID_AVAIL, WL_SCAN_COMPLETED,
    // WL_CONNECTED, WL_CONNECT_FAILED, WL_CONNECTION_LOST, WL_DISCONNECTED,
    // WL_AP_LISTENING, WL_AP_CONNECTED, WL_AP_FAILED"""
    // resp = self._send_command_get_response(_GET_CONN_STATUS_CMD)
    // if self._debug:
    //     print("Connection status:", resp[0][0])
    // return resp[0][0]  # one byte response
    return mp_const_none;
}

mp_obj_t esp32spi_firmware_version() {
    // """A string of the firmware version on the ESP32"""
    // if self._debug:
    //     print("Firmware version")
    // resp = self._send_command_get_response(_GET_FW_VERSION_CMD)
    // return resp[0]
    return mp_const_none;
}

mp_obj_t esp32spi_MAC_address() {
    // """A bytearray containing the MAC address of the ESP32"""
    // if self._debug:
    //     print("MAC address")
    // resp = self._send_command_get_response(_GET_MACADDR_CMD, [b"\xFF"])
    // return resp[0]
    return mp_const_none;
}

mp_obj_t esp32spi_MAC_address_actual() {
    // """A bytearray containing the actual MAC address of the ESP32"""
    // if self._debug:
    //     print("MAC address")
    // resp = self._send_command_get_response(_GET_MACADDR_CMD, [b"\xFF"])
    // new_resp = bytearray(resp[0])
    // new_resp = reversed(new_resp)
    // return new_resp
    return mp_const_none;
}

mp_obj_t esp32spi_start_scan_networks() {
    // """Begin a scan of visible access points. Follow up with a call
    // to 'get_scan_networks' for response"""
    // if self._debug:
    //     print("Start scan")
    // resp = self._send_command_get_response(_START_SCAN_NETWORKS)
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to start AP scan")
    return mp_const_none;
}

mp_obj_t esp32spi_get_scan_networks() {
    // """The results of the latest SSID scan. Returns a list of dictionaries with
    // 'ssid', 'rssi', 'encryption', bssid, and channel entries, one for each AP found"""
    // self._send_command(_SCAN_NETWORKS)
    // names = self._wait_response_cmd(_SCAN_NETWORKS)
    // # print("SSID names:", names)
    // APs = []  # pylint: disable=invalid-name
    // for i, name in enumerate(names):
    //     a_p = {"ssid": name}
    //     rssi = self._send_command_get_response(_GET_IDX_RSSI_CMD, ((i,),))[0]
    //     a_p["rssi"] = struct.unpack("<i", rssi)[0]
    //     encr = self._send_command_get_response(_GET_IDX_ENCT_CMD, ((i,),))[0]
    //     a_p["encryption"] = encr[0]
    //     bssid = self._send_command_get_response(_GET_IDX_BSSID_CMD, ((i,),))[0]
    //     a_p["bssid"] = bssid
    //     chan = self._send_command_get_response(_GET_IDX_CHAN_CMD, ((i,),))[0]
    //     a_p["channel"] = chan[0]
    //     APs.append(a_p)
    // return APs
    return mp_const_none;
}

mp_obj_t esp32spi_scan_networks() {
    // """Scan for visible access points, returns a list of access point details.
    // Returns a list of dictionaries with 'ssid', 'rssi' and 'encryption' entries,
    // one for each AP found"""
    // self.start_scan_networks()
    // for _ in range(10):  # attempts
    //     time.sleep(2)
    //     APs = self.get_scan_networks()  # pylint: disable=invalid-name
    //     if APs:
    //         return APs
    // return None
    return mp_const_none;
}

mp_obj_t esp32spi_wifi_set_network(mp_obj_t ssid) {
    // """Tells the ESP32 to set the access point to the given ssid"""
    // resp = self._send_command_get_response(_SET_NET_CMD, [ssid])
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set network")
    return mp_const_none;
}

mp_obj_t esp32spi_wifi_set_passphrase(mp_obj_t ssid, mp_obj_t passphrase) {
    // """Sets the desired access point ssid and passphrase"""
    // resp = self._send_command_get_response(_SET_PASSPHRASE_CMD, [ssid, passphrase])
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set passphrase")
    return mp_const_none;
}

mp_obj_t esp32spi_wifi_set_entidentity(mp_obj_t ident) {
    // """Sets the WPA2 Enterprise anonymous identity"""
    // resp = self._send_command_get_response(_SET_ENT_IDENT_CMD, [ident])
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set enterprise anonymous identity")
    return mp_const_none;
}

mp_obj_t esp32spi_wifi_set_entusername(mp_obj_t username) {
    // """Sets the desired WPA2 Enterprise username"""
    // resp = self._send_command_get_response(_SET_ENT_UNAME_CMD, [username])
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set enterprise username")
    return mp_const_none;
}

mp_obj_t esp32spi_wifi_set_entpassword(mp_obj_t password) {
    // """Sets the desired WPA2 Enterprise password"""
    // resp = self._send_command_get_response(_SET_ENT_PASSWD_CMD, [password])
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set enterprise password")
    return mp_const_none;
}

mp_obj_t esp32spi_wifi_set_entenable() {
    // """Enables WPA2 Enterprise mode"""
    // resp = self._send_command_get_response(_SET_ENT_ENABLE_CMD)
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to enable enterprise mode")
    return mp_const_none;
}

mp_obj_t esp32spi_ssid() {
    // """The name of the access point we're connected to"""
    // resp = self._send_command_get_response(_GET_CURR_SSID_CMD, [b"\xFF"])
    // return resp[0]
    return mp_const_none;
}

mp_obj_t esp32spi_bssid() {
    // """The MAC-formatted service set ID of the access point we're connected to"""
    // resp = self._send_command_get_response(_GET_CURR_BSSID_CMD, [b"\xFF"])
    // return resp[0]
    return mp_const_none;
}

mp_obj_t esp32spi_rssi() {
    // """The receiving signal strength indicator for the access point we're
    // connected to"""
    // resp = self._send_command_get_response(_GET_CURR_RSSI_CMD, [b"\xFF"])
    // return struct.unpack("<i", resp[0])[0]
    return mp_const_none;
}

mp_obj_t esp32spi_network_data() {
    // """A dictionary containing current connection details such as the 'ip_addr',
    // 'netmask' and 'gateway'"""
    // resp = self._send_command_get_response(
    //     _GET_IPADDR_CMD, [b"\xFF"], reply_params=3
    // )
    // return {"ip_addr": resp[0], "netmask": resp[1], "gateway": resp[2]}
    return mp_const_none;
}

mp_obj_t esp32spi_ip_address() {
    // """Our local IP address"""
    // return self.network_data["ip_addr"]
    return mp_const_none;
}

mp_obj_t esp32spi_is_connected() {
    // """Whether the ESP32 is connected to an access point"""
    // try:
    //     return self.status == WL_CONNECTED
    // except RuntimeError:
    //     self.reset()
    //     return False
    return mp_const_none;
}

mp_obj_t esp32spi_ap_listening() {
    // """Returns if the ESP32 is in access point mode and is listening for connections"""
    // try:
    //     return self.status == WL_AP_LISTENING
    // except RuntimeError:
    //     self.reset()
    //     return False
    return mp_const_none;
}

mp_obj_t esp32spi_disconnect() {
    // """Disconnect from the access point"""
    // resp = self._send_command_get_response(_DISCONNECT_CMD)
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to disconnect")
    return mp_const_none;
}

mp_obj_t esp32spi_connect(mp_obj_t secrets) {
    // """Connect to an access point using a secrets dictionary
    // that contains a 'ssid' and 'password' entry"""
    // self.connect_AP(secrets["ssid"], secrets["password"])
    return mp_const_none;
}

mp_obj_t esp32spi_connect_AP(mp_obj_t ssid, mp_obj_t password, mp_obj_t timeout_s/* = 10*/) {
    // """
    // Connect to an access point with given name and password.
    // Will wait until specified timeout seconds and return on success
    // or raise an exception on failure.
    // :param ssid: the SSID to connect to
    // :param passphrase: the password of the access point
    // :param timeout_s: number of seconds until we time out and fail to create AP
    // """
    // if self._debug:
    //     print("Connect to AP", ssid, password)
    // if isinstance(ssid, str):
    //     ssid = bytes(ssid, "utf-8")
    // if password:
    //     if isinstance(password, str):
    //         password = bytes(password, "utf-8")
    //     self.wifi_set_passphrase(ssid, password)
    // else:
    //     self.wifi_set_network(ssid)
    // times = time.monotonic()
    // while (time.monotonic() - times) < timeout_s:  # wait up until timeout
    //     stat = self.status
    //     if stat == WL_CONNECTED:
    //         return stat
    //     time.sleep(0.05)
    // if stat in (WL_CONNECT_FAILED, WL_CONNECTION_LOST, WL_DISCONNECTED):
    //     raise RuntimeError("Failed to connect to ssid", ssid)
    // if stat == WL_NO_SSID_AVAIL:
    //     raise RuntimeError("No such ssid", ssid)
    // raise RuntimeError("Unknown error 0x%02X" % stat)
    return mp_const_none;
}

mp_obj_t esp32spi_create_AP(mp_uint_t n_args, const mp_obj_t *args) {
    // """
    // Create an access point with the given name, password, and channel.
    // Will wait until specified timeout seconds and return on success
    // or raise an exception on failure.
    // :param str ssid: the SSID of the created Access Point. Must be less than 32 chars.
    // :param str password: the password of the created Access Point. Must be 8-63 chars.
    // :param int channel: channel of created Access Point (1 - 14).
    // :param int timeout: number of seconds until we time out and fail to create AP
    // """
    // if len(ssid) > 32:
    //     raise RuntimeError("ssid must be no more than 32 characters")
    // if password and (len(password) < 8 or len(password) > 64):
    //     raise RuntimeError("password must be 8 - 63 characters")
    // if channel < 1 or channel > 14:
    //     raise RuntimeError("channel must be between 1 and 14")

    // if isinstance(channel, int):
    //     channel = bytes(channel)
    // if isinstance(ssid, str):
    //     ssid = bytes(ssid, "utf-8")
    // if password:
    //     if isinstance(password, str):
    //         password = bytes(password, "utf-8")
    //     self._wifi_set_ap_passphrase(ssid, password, channel)
    // else:
    //     self._wifi_set_ap_network(ssid, channel)

    // times = time.monotonic()
    // while (time.monotonic() - times) < timeout:  # wait up to timeout
    //     stat = self.status
    //     if stat == WL_AP_LISTENING:
    //         return stat
    //     time.sleep(0.05)
    // if stat == WL_AP_FAILED:
    //     raise RuntimeError("Failed to create AP", ssid)
    // raise RuntimeError("Unknown error 0x%02x" % stat)
    return mp_const_none;
}

mp_obj_t esp32spi_pretty_ip(mp_obj_t ip) {
    // """Converts a bytearray IP address to a dotted-quad string for printing"""
    // return "%d.%d.%d.%d" % (ip[0], ip[1], ip[2], ip[3])
    return mp_const_none;
}

mp_obj_t esp32spi_unpretty_ip(mp_obj_t ip) {
    // """Converts a dotted-quad string to a bytearray IP address"""
    // octets = [int(x) for x in ip.split(".")]
    // return bytes(octets)
    return mp_const_none;
}

mp_obj_t esp32spi_get_host_by_name(mp_obj_t hostname) {
    // """Convert a hostname to a packed 4-byte IP address. Returns
    // a 4 bytearray"""
    // if self._debug:
    //     print("*** Get host by name")
    // if isinstance(hostname, str):
    //     hostname = bytes(hostname, "utf-8")
    // resp = self._send_command_get_response(_REQ_HOST_BY_NAME_CMD, (hostname,))
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to request hostname")
    // resp = self._send_command_get_response(_GET_HOST_BY_NAME_CMD)
    // return resp[0]
    return mp_const_none;
}

mp_obj_t esp32spi_ping(mp_obj_t dest, mp_obj_t ttl/* = 250*/) {
    // """Ping a destination IP address or hostname, with a max time-to-live
    // (ttl). Returns a millisecond timing value"""
    // if isinstance(dest, str):  # convert to IP address
    //     dest = self.get_host_by_name(dest)
    // # ttl must be between 0 and 255
    // ttl = max(0, min(ttl, 255))
    // resp = self._send_command_get_response(_PING_CMD, (dest, (ttl,)))
    // return struct.unpack("<H", resp[0])[0]
    return mp_const_none;
}

mp_obj_t esp32spi_get_socket() {
    // """Request a socket from the ESP32, will allocate and return a number that
    // can then be passed to the other socket commands"""
    // if self._debug:
    //     print("*** Get socket")
    // resp = self._send_command_get_response(_GET_SOCKET_CMD)
    // resp = resp[0][0]
    // if resp == 255:
    //     raise OSError(23)  # ENFILE - File table overflow
    // if self._debug:
    //     print("Allocated socket #%d" % resp)
    // return resp
    return mp_const_none;
}

mp_obj_t esp32spi_socket_open(mp_uint_t n_args, const mp_obj_t *args) {
    // """Open a socket to a destination IP address or hostname
    // using the ESP32's internal reference number. By default we use
    // 'conn_mode' TCP_MODE but can also use UDP_MODE or TLS_MODE
    // (dest must be hostname for TLS_MODE!)"""
    // self._socknum_ll[0][0] = socket_num
    // if self._debug:
    //     print("*** Open socket to", dest, port, conn_mode)
    // if conn_mode == ESP_SPIcontrol.TLS_MODE and self._tls_socket is not None:
    //     raise OSError(23)  # ENFILE - File table overflow
    // port_param = struct.pack(">H", port)
    // if isinstance(dest, str):  # use the 5 arg version
    //     dest = bytes(dest, "utf-8")
    //     resp = self._send_command_get_response(
    //         _START_CLIENT_TCP_CMD,
    //         (
    //             dest,
    //             b"\x00\x00\x00\x00",
    //             port_param,
    //             self._socknum_ll[0],
    //             (conn_mode,),
    //         ),
    //     )
    // else:  # ip address, use 4 arg vesion
    //     resp = self._send_command_get_response(
    //         _START_CLIENT_TCP_CMD,
    //         (dest, port_param, self._socknum_ll[0], (conn_mode,)),
    //     )
    // if resp[0][0] != 1:
    //     raise RuntimeError("Could not connect to remote server")
    // if conn_mode == ESP_SPIcontrol.TLS_MODE:
    //     self._tls_socket = socket_num
    return mp_const_none;
}

mp_obj_t esp32spi_socket_status(mp_obj_t socket_num) {
    // """Get the socket connection status, can be SOCKET_CLOSED, SOCKET_LISTEN,
    // SOCKET_SYN_SENT, SOCKET_SYN_RCVD, SOCKET_ESTABLISHED, SOCKET_FIN_WAIT_1,
    // SOCKET_FIN_WAIT_2, SOCKET_CLOSE_WAIT, SOCKET_CLOSING, SOCKET_LAST_ACK, or
    // SOCKET_TIME_WAIT"""
    // self._socknum_ll[0][0] = socket_num
    // resp = self._send_command_get_response(
    //     _GET_CLIENT_STATE_TCP_CMD, self._socknum_ll
    // )
    // return resp[0][0]
    return mp_const_none;
}

mp_obj_t esp32spi_socket_connected(mp_obj_t socket_num) {
    // """Test if a socket is connected to the destination, returns boolean true/false"""
    // return self.socket_status(socket_num) == SOCKET_ESTABLISHED
    return mp_const_none;
}

mp_obj_t esp32spi_socket_write(mp_obj_t socket_num, mp_obj_t buffer, mp_obj_t conn_mode/* = TCP_MODE*/) {
    // """Write the bytearray buffer to a socket"""
    // if self._debug:
    //     print("Writing:", buffer)
    // self._socknum_ll[0][0] = socket_num
    // sent = 0
    // total_chunks = (len(buffer) // 64) + 1
    // send_command = _SEND_DATA_TCP_CMD
    // if conn_mode == self.UDP_MODE:  # UDP requires a different command to write
    //     send_command = _INSERT_DATABUF_TCP_CMD
    // for chunk in range(total_chunks):
    //     resp = self._send_command_get_response(
    //         send_command,
    //         (
    //             self._socknum_ll[0],
    //             memoryview(buffer)[(chunk * 64) : ((chunk + 1) * 64)],
    //         ),
    //         sent_param_len_16=True,
    //     )
    //     sent += resp[0][0]

    // if conn_mode == self.UDP_MODE:
    //     # UDP verifies chunks on write, not bytes
    //     if sent != total_chunks:
    //         raise RuntimeError(
    //             "Failed to write %d chunks (sent %d)" % (total_chunks, sent)
    //         )
    //     # UDP needs to finalize with this command, does the actual sending
    //     resp = self._send_command_get_response(_SEND_UDP_DATA_CMD, self._socknum_ll)
    //     if resp[0][0] != 1:
    //         raise RuntimeError("Failed to send UDP data")
    //     return

    // if sent != len(buffer):
    //     self.socket_close(socket_num)
    //     raise RuntimeError(
    //         "Failed to send %d bytes (sent %d)" % (len(buffer), sent)
    //     )

    // resp = self._send_command_get_response(_DATA_SENT_TCP_CMD, self._socknum_ll)
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to verify data sent")
    return mp_const_none;
}

mp_obj_t esp32spi_socket_available(mp_obj_t socket_num) {
    // """Determine how many bytes are waiting to be read on the socket"""
    // self._socknum_ll[0][0] = socket_num
    // resp = self._send_command_get_response(_AVAIL_DATA_TCP_CMD, self._socknum_ll)
    // reply = struct.unpack("<H", resp[0])[0]
    // if self._debug:
    //     print("ESPSocket: %d bytes available" % reply)
    // return reply
    return mp_const_none;
}

mp_obj_t esp32spi_socket_read(mp_obj_t socket_num, mp_obj_t size) {
    // """Read up to 'size' bytes from the socket number. Returns a bytearray"""
    // if self._debug:
    //     print(
    //         "Reading %d bytes from ESP socket with status %d"
    //         % (size, self.socket_status(socket_num))
    //     )
    // self._socknum_ll[0][0] = socket_num
    // resp = self._send_command_get_response(
    //     _GET_DATABUF_TCP_CMD,
    //     (self._socknum_ll[0], (size & 0xFF, (size >> 8) & 0xFF)),
    //     sent_param_len_16=True,
    //     recv_param_len_16=True,
    // )
    // return bytes(resp[0])
    return mp_const_none;
}

mp_obj_t esp32spi_socket_connect(mp_uint_t n_args, const mp_obj_t *args) {
    // """Open and verify we connected a socket to a destination IP address or hostname
    // using the ESP32's internal reference number. By default we use
    // 'conn_mode' TCP_MODE but can also use UDP_MODE or TLS_MODE (dest must
    // be hostname for TLS_MODE!)"""
    // if self._debug:
    //     print("*** Socket connect mode", conn_mode)

    // self.socket_open(socket_num, dest, port, conn_mode=conn_mode)
    // if conn_mode == self.UDP_MODE:
    //     # UDP doesn't actually establish a connection
    //     # but the socket for writing is created via start_server
    //     self.start_server(port, socket_num, conn_mode)
    //     return True

    // times = time.monotonic()
    // while (time.monotonic() - times) < 3:  # wait 3 seconds
    //     if self.socket_connected(socket_num):
    //         return True
    //     time.sleep(0.01)
    // raise RuntimeError("Failed to establish connection")
    return mp_const_none;
}

mp_obj_t esp32spi_socket_close(mp_obj_t socket_num) {
    // """Close a socket using the ESP32's internal reference number"""
    // if self._debug:
    //     print("*** Closing socket #%d" % socket_num)
    // self._socknum_ll[0][0] = socket_num
    // try:
    //     self._send_command_get_response(_STOP_CLIENT_TCP_CMD, self._socknum_ll)
    // except RuntimeError:
    //     pass
    // if socket_num == self._tls_socket:
    //     self._tls_socket = None
    return mp_const_none;
}

mp_obj_t esp32spi_start_server(mp_uint_t n_args, const mp_obj_t *args) {
    // """Opens a server on the specified port, using the ESP32's internal reference number"""
    // if self._debug:
    //     print("*** starting server")
    // self._socknum_ll[0][0] = socket_num
    // params = [struct.pack(">H", port), self._socknum_ll[0], (conn_mode,)]
    // if ip:
    //     params.insert(0, ip)
    // resp = self._send_command_get_response(_START_SERVER_TCP_CMD, params)

    // if resp[0][0] != 1:
    //     raise RuntimeError("Could not start server")
    return mp_const_none;
}

mp_obj_t esp32spi_server_state(mp_obj_t socket_num) {
    // """Get the state of the ESP32's internal reference server socket number"""
    // self._socknum_ll[0][0] = socket_num
    // resp = self._send_command_get_response(_GET_STATE_TCP_CMD, self._socknum_ll)
    // return resp[0][0]
    return mp_const_none;
}

mp_obj_t esp32spi_esp_debug(mp_obj_t enabled) {
    // """Enable/disable debug mode on the ESP32. Debug messages will be
    // written to the ESP32's UART."""
    // resp = self._send_command_get_response(_SET_DEBUG_CMD, ((bool(enabled),),))
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set debug mode")
    return mp_const_none;
}

mp_obj_t esp32spi_set_pin_mode(mp_obj_t pin, mp_obj_t mode) {
    // """
    // Set the io mode for a GPIO pin.
    // :param int pin: ESP32 GPIO pin to set.
    // :param value: direction for pin, digitalio.Direction or integer (0=input, 1=output).
    // """
    // if mode == Direction.OUTPUT:
    //     pin_mode = 1
    // elif mode == Direction.INPUT:
    //     pin_mode = 0
    // else:
    //     pin_mode = mode
    // resp = self._send_command_get_response(_SET_PIN_MODE_CMD, ((pin,), (pin_mode,)))
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set pin mode")
    return mp_const_none;
}

mp_obj_t esp32spi_set_digital_write(mp_obj_t pin, mp_obj_t value) {
    // """
    // Set the digital output value of pin.
    // :param int pin: ESP32 GPIO pin to write to.
    // :param bool value: Value for the pin.
    // """
    // resp = self._send_command_get_response(
    //     _SET_DIGITAL_WRITE_CMD, ((pin,), (value,))
    // )
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to write to pin")
    return mp_const_none;
}

mp_obj_t esp32spi_set_analog_write(mp_obj_t pin, mp_obj_t analog_value) {
    // """
    // Set the analog output value of pin, using PWM.
    // :param int pin: ESP32 GPIO pin to write to.
    // :param float value: 0=off 1.0=full on
    // """
    // value = int(255 * analog_value)
    // resp = self._send_command_get_response(
    //     _SET_ANALOG_WRITE_CMD, ((pin,), (value,))
    // )
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to write to pin")
    return mp_const_none;
}

mp_obj_t esp32spi_set_digital_read(mp_obj_t pin) {
    // """
    // Get the digital input value of pin. Returns the boolean value of the pin.
    // :param int pin: ESP32 GPIO pin to read from.
    // """
    // # Verify nina-fw => 1.5.0
    // fw_semver_maj = bytes(self.firmware_version).decode("utf-8")[2]
    // assert int(fw_semver_maj) >= 5, "Please update nina-fw to 1.5.0 or above."

    // resp = self._send_command_get_response(_SET_DIGITAL_READ_CMD, ((pin,),))[0]
    // if resp[0] == 0:
    //     return False
    // if resp[0] == 1:
    //     return True
    // raise ValueError(
    //     "_SET_DIGITAL_READ response error: response is not boolean", resp[0]
    // )
    return mp_const_none;
}

mp_obj_t esp32spi_set_analog_read(mp_obj_t pin, mp_obj_t atten/* = ADC_ATTEN_DB_11*/) {
    // """
    // Get the analog input value of pin. Returns an int between 0 and 65536.
    // :param int pin: ESP32 GPIO pin to read from.
    // :param int atten: attenuation constant
    // """
    // # Verify nina-fw => 1.5.0
    // fw_semver_maj = bytes(self.firmware_version).decode("utf-8")[2]
    // assert int(fw_semver_maj) >= 5, "Please update nina-fw to 1.5.0 or above."

    // resp = self._send_command_get_response(_SET_ANALOG_READ_CMD, ((pin,), (atten,)))
    // resp_analog = struct.unpack("<i", resp[0])
    // if resp_analog[0] < 0:
    //     raise ValueError(
    //         "_SET_ANALOG_READ parameter error: invalid pin", resp_analog[0]
    //     )
    // if self._debug:
    //     print(resp, resp_analog, resp_analog[0], 16 * resp_analog[0])
    // return 16 * resp_analog[0]
    return mp_const_none;
}

mp_obj_t esp32spi_get_time() {
    // """The current unix timestamp"""
    // if self.status == WL_CONNECTED:
    //     resp = self._send_command_get_response(_GET_TIME)
    //     resp_time = struct.unpack("<i", resp[0])
    //     if resp_time == (0,):
    //         raise ValueError("_GET_TIME returned 0")
    //     return resp_time
    // if self.status in (WL_AP_LISTENING, WL_AP_CONNECTED):
    //     raise RuntimeError(
    //         "Cannot obtain NTP while in AP mode, must be connected to internet"
    //     )
    // raise RuntimeError("Must be connected to WiFi before obtaining NTP.")
    return mp_const_none;
}

mp_obj_t esp32spi_set_certificate(mp_obj_t client_certificate) {
    // """Sets client certificate. Must be called
    // BEFORE a network connection is established.
    // :param str client_certificate: User-provided .PEM certificate up to 1300 bytes.
    // """
    // if self._debug:
    //     print("** Setting client certificate")
    // if self.status == WL_CONNECTED:
    //     raise RuntimeError(
    //         "set_certificate must be called BEFORE a connection is established."
    //     )
    // if isinstance(client_certificate, str):
    //     client_certificate = bytes(client_certificate, "utf-8")
    // if "-----BEGIN CERTIFICATE" not in client_certificate:
    //     raise TypeError(".PEM must start with -----BEGIN CERTIFICATE")
    // assert len(client_certificate) < 1300, ".PEM must be less than 1300 bytes."
    // resp = self._send_command_get_response(_SET_CLI_CERT, (client_certificate,))
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set client certificate")
    // self.set_crt = True
    // return resp[0]
    return mp_const_none;
}

mp_obj_t esp32spi_set_private_key(mp_obj_t private_key) {
    // """Sets private key. Must be called
    // BEFORE a network connection is established.
    // :param str private_key: User-provided .PEM file up to 1700 bytes.
    // """
    // if self._debug:
    //     print("** Setting client's private key.")
    // if self.status == WL_CONNECTED:
    //     raise RuntimeError(
    //         "set_private_key must be called BEFORE a connection is established."
    //     )
    // if isinstance(private_key, str):
    //     private_key = bytes(private_key, "utf-8")
    // if "-----BEGIN RSA" not in private_key:
    //     raise TypeError(".PEM must start with -----BEGIN RSA")
    // assert len(private_key) < 1700, ".PEM must be less than 1700 bytes."
    // resp = self._send_command_get_response(_SET_PK, (private_key,))
    // if resp[0][0] != 1:
    //     raise RuntimeError("Failed to set private key.")
    // self.set_psk = True
    // return resp[0]
    return mp_const_none;
}


// mp_obj_t esp32spi_get_width() {
//     return mp_obj_new_int(ESP32SPI::WIDTH);
// }

// mp_obj_t esp32spi_get_height() {
//     return mp_obj_new_int(ESP32SPI::HEIGHT);
// }

// mp_obj_t esp32spi_update() {
//     if(scroll != nullptr)
//         scroll->update();
//     else
//         mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
//     return mp_const_none;
// }

// mp_obj_t esp32spi_set_pixel(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t v_obj) {
//     if(scroll != nullptr) {
//         int x = mp_obj_get_int(x_obj);
//         int y = mp_obj_get_int(y_obj);
//         int val = mp_obj_get_int(v_obj);

//         if(x < 0 || x >= ESP32SPI::WIDTH || y < 0 || y >= ESP32SPI::HEIGHT)
//             mp_raise_ValueError("x or y out of range.");
//         else {
//             if(val < 0 || val > 255)
//                 mp_raise_ValueError("val out of range. Expected 0 to 255");
//             else
//                 scroll->set_pixel(x, y, val);
//         }
//     }
//     else
//         mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

//     return mp_const_none;
// }

// mp_obj_t esp32spi_clear() {
//     if(scroll != nullptr)
//         scroll->clear();
//     else
//         mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
//     return mp_const_none;
// }

// mp_obj_t esp32spi_is_pressed(mp_obj_t button_obj) {
//     bool buttonPressed = false;
    
//     if(scroll != nullptr) {
//         int buttonID = mp_obj_get_int(button_obj);    
//         switch(buttonID) {
//         case 0:
//             buttonPressed = scroll->is_pressed(ESP32SPI::A);
//             break;

//         case 1:
//             buttonPressed = scroll->is_pressed(ESP32SPI::B);
//             break;

//         case 2:
//             buttonPressed = scroll->is_pressed(ESP32SPI::X);
//             break;

//         case 3:
//             buttonPressed = scroll->is_pressed(ESP32SPI::Y);
//             break;

//         default:
//             mp_raise_ValueError("button not valid. Expected 0 to 3");
//             break;
//         }
//     }
//     else
//         mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

//     return buttonPressed ? mp_const_true : mp_const_false;
// }
}