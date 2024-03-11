#include <cstdio>

#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)

extern "C" {
#include "lte.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "extmod/modnetwork.h"

typedef struct _lte_obj_t {
    mp_obj_base_t base;
} lte_obj_t;

mp_obj_t network_lte_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _lte_obj_t *self = nullptr;

    enum { 
        ARG_dummy
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dummy, MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj_with_finaliser(_lte_obj_t);
    self->base.type = &mod_network_nic_type_lte;

    // Register with network module
    mod_network_register_nic(MP_OBJ_FROM_PTR(self));

    return MP_OBJ_FROM_PTR(self);
}

int network_lte_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    debug_printf("gethostbyname(%s)\n", name);
    return -1;
}

void network_lte_deinit(void) {
}

int network_lte_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    debug_printf("socket_socket(%d %d %d)\n", socket->domain, socket->type, socket->proto);
    *_errno = MP_EINVAL;
    return -1;
}

void network_lte_socket_close(mod_network_socket_obj_t *socket) {
    debug_printf("socket_close(%d)\n", socket->fileno);
};

int network_lte_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_bind(%d, %d)\n", socket->fileno, port);
    *_errno = MP_EINVAL;
    return -1;
};

int network_lte_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    debug_printf("socket_listen(%d, %d)\n", socket->fileno, backlog);
    *_errno = MP_EINVAL;
    return -1;
};

int network_lte_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    debug_printf("socket_accept(%d)\n", socket->fileno);
    *_errno = MP_EINVAL;
    return -1;
};

int network_lte_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
};

mp_uint_t network_lte_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    debug_printf("socket_send(%d, %d)\n", socket->fileno, len);
    *_errno = MP_EINVAL;
    return 0;;
};

mp_uint_t network_lte_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    debug_printf("socket_recv(%d)\n", socket->fileno);
    *_errno = MP_EINVAL;
    return 0;
};

mp_uint_t network_lte_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_sendto(%d)\n", socket->fileno);
    *_errno = MP_EINVAL;
    return 0;
};

mp_uint_t network_lte_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    debug_printf("socket_recvfrom(%d)\n", socket->fileno);
    *_errno = MP_EINVAL;
    return 0;
};

int network_lte_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    debug_printf("socket_setsockopt(%d, %d)\n", socket->fileno, opt);
    *_errno = MP_EINVAL;
    return -1;
};

int network_lte_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    debug_printf("socket_settimeout(%d, %d)\n", socket->fileno, timeout_ms);
    *_errno = MP_EINVAL;
    return -1;
};

int network_lte_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    debug_printf("socket_ioctl(%d, %d)\n", socket->fileno, request);
    *_errno = MP_EINVAL;
    return -1;
};

}