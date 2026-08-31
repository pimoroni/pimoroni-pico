// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

#include "extmod/modnetwork.h"

extern const mp_obj_type_t mod_network_nic_type_pint;

extern mp_obj_t network_pint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
extern mp_obj_t network_pint__del__(mp_obj_t self_in);

extern int network_pint_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip);
extern void network_pint_deinit(void);
extern int network_pint_socket_socket(mod_network_socket_obj_t *socket, int *_errno);
extern void network_pint_socket_close(mod_network_socket_obj_t *socket);
extern int network_pint_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno);
extern int network_pint_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno);
extern int network_pint_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno);
extern int network_pint_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno);
extern mp_uint_t network_pint_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno);
extern mp_uint_t network_pint_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno);
extern mp_uint_t network_pint_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno);
extern mp_uint_t network_pint_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno);
extern int network_pint_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno);
extern int network_pint_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno);
extern int network_pint_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno);
