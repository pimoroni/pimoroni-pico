# P.I.N.T

### Python Implementation of Network Transports.

Want to glue your esoteric network device into MicroPython without delving
into C or trying to figure out lwIP? PINT is just the refreshment you need.

### Reference Implementation

PINT uses a Python class to implement a socket-based networking driver.

How you implement these functions is down to you- but you must expect/return
the correct data. Here's the basic skeleton:

```python
class PINT_Socket():
    """An arbitrary structure for storing data about your sockets.

    Does not have to be a class, you could just return an int with the
    socket ID of your target device.
    """
    def __init__(self):
        pass


class PINT_NIC:
    """The actual NIC implementation.

    Most of the heavy lifting is sockets based, you might want to implement
    the socket_ methods on your socket class and simply delegate to them.
    """
    def __init__(self) -> None:
        pass

    def __del__(self) -> None:
        pass

    def gethostbyname(self, name: str) -> tuple[int, int, int, int]:
        return (127, 0, 0, 1)

    def socket_socket(self) -> PINT_Socket:
        return PINT_Socket()

    def socket_close(self, socket: PINT_Socket) -> None:
        pass

    def socket_bind(self, socket: PINT_Socket, ip: tuple[int, int, int, int], port: int) -> bool:
        return True

    def socket_listen(self, socket: PINT_Socket, backlog: int) -> bool:
        return True

    def socket_accept(self, socket: PINT_Socket, socket2: PINT_Socket, ip: tuple[int, int, int, int], port: int) -> bool:
        return True

    def socket_connect(self, socket: PINT_Socket, ip: tuple[int, int, int, int], port) -> bool:
        return True

    def socket_send(self, socket: PINT_Socket, buf: bytearray) -> int:
        return 0

    def socket_recv(self, socket: PINT_Socket, buf: bytearray) -> int:
        """Buf is provided as a mutable bytearray, you must write into it."""
        return 0

    def socket_sendto(self, socket: PINT_Socket, buf: bytearray, ip, port) -> int:
        return 0

    def socket_recvfrom(self, socket: PINT_Socket, buf: bytearray, ip, port) -> int:
        """Buf is provided as a mutable bytearray, you must write into it."""
        return 0

    def socket_setsockopt(self, socket: PINT_Socket, level: int, opt: int, val: bytearray) -> bool:
        return True

    def socket_settimeout(self, socket: PINT_Socket, timeout_ms: int) -> bool:
        return True

    def socket_ioctl(self, socket: PINT_Socket, request: int, arg: int) -> bool:
        return True

```