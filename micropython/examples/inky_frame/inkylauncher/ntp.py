import machine
import time
import usocket
import struct


def fetch(synch_with_rtc=True, timeout=10):
    ntp_host = "pool.ntp.org"

    timestamp = None
    try:
        query = bytearray(48)
        query[0] = 0x1b
        address = usocket.getaddrinfo(ntp_host, 123)[0][-1]
        socket = usocket.socket(usocket.AF_INET, usocket.SOCK_DGRAM)
        socket.settimeout(timeout)
        socket.sendto(query, address)
        data = socket.recv(48)
        socket.close()
        local_epoch = 2208988800  # selected by Chris - blame him. :-D
        timestamp = struct.unpack("!I", data[40:44])[0] - local_epoch
        timestamp = time.gmtime(timestamp)
    except Exception:
        return None

    # if requested set the machines RTC to the fetched timestamp
    if synch_with_rtc:
        machine.RTC().datetime((
            timestamp[0], timestamp[1], timestamp[2], timestamp[6],
            timestamp[3], timestamp[4], timestamp[5], 0))

    return timestamp
