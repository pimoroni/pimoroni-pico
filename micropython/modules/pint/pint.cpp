#include <cstdio>
#include <stdarg.h>
#include "micropython/modules/util.hpp"

//#define debug_printf(...) mp_printf(&mp_plat_print, "PINT: " __VA_ARGS__)
#define debug_printf(...) (void)0

extern "C" {
#include "pint.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "extmod/modnetwork.h"

typedef struct _pint_obj_t {
    mp_obj_base_t base;
    mp_obj_t __del__[2];
    mp_obj_t gethostbyname[2];
    mp_obj_t socket_socket[2];
    mp_obj_t socket_close[2];
    mp_obj_t socket_bind[2];
    mp_obj_t socket_listen[2];
    mp_obj_t socket_accept[2];
    mp_obj_t socket_connect[2];
    mp_obj_t socket_send[2];
    mp_obj_t socket_recv[2];
    mp_obj_t socket_sendto[2];
    mp_obj_t socket_recvfrom[2];
    mp_obj_t socket_setsockopt[2];
    mp_obj_t socket_settimeout[2];
    mp_obj_t socket_ioctl[2];
} pint_obj_t;

void bind_method(mp_obj_t impl, qstr name, mp_obj_t *dest, bool required=true) {
    if(required) {
        mp_load_method(impl, name, dest);
    } else {
        mp_load_method_maybe(impl, name, dest);
        if(dest[0] == MP_OBJ_NULL) debug_printf("missing %s\n", qstr_str(name));
    }
}

mp_obj_t call_method(mp_obj_t *method, uint8_t count=0, ...) {
    mp_obj_t kwargs[10];

    kwargs[0] = method[0];
    kwargs[1] = method[1];

    va_list args;

    va_start(args, count);

    for(auto i = 0u; i < count; i++) {
        kwargs[i + 2] = va_arg(args, mp_obj_t);
    }

    va_end(args);

    return mp_call_method_n_kw(count, 0, kwargs);
}

mp_obj_t network_pint__del__(mp_obj_t nic_in) {
    pint_obj_t *nic = (pint_obj_t *)MP_OBJ_TO_PTR(nic_in);
    call_method(nic->__del__);
    return mp_const_none;
}

mp_obj_t network_pint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _pint_obj_t *nic = nullptr;

    enum { 
        ARG_impl
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_impl, MP_ARG_OBJ},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    nic = m_new_obj_with_finaliser(_pint_obj_t);
    nic->base.type = &mod_network_nic_type_pint;

    mp_obj_t impl = args[ARG_impl].u_obj;

    // Grab each method, this stores the method pointer and self in an mp_obj_t[2] for dispatch
    bind_method(impl, MP_QSTR___del__, nic->__del__);
    bind_method(impl, MP_QSTR_gethostbyname, nic->gethostbyname);
    bind_method(impl, MP_QSTR_socket_socket, nic->socket_socket);
    bind_method(impl, MP_QSTR_socket_close, nic->socket_close);
    bind_method(impl, MP_QSTR_socket_bind, nic->socket_bind);
    bind_method(impl, MP_QSTR_socket_listen, nic->socket_listen);
    bind_method(impl, MP_QSTR_socket_accept, nic->socket_accept);
    bind_method(impl, MP_QSTR_socket_connect, nic->socket_connect);
    bind_method(impl, MP_QSTR_socket_send, nic->socket_send);
    bind_method(impl, MP_QSTR_socket_recv, nic->socket_recv);
    bind_method(impl, MP_QSTR_socket_sendto, nic->socket_sendto);
    bind_method(impl, MP_QSTR_socket_recvfrom, nic->socket_recvfrom);
    bind_method(impl, MP_QSTR_socket_setsockopt, nic->socket_setsockopt);
    bind_method(impl, MP_QSTR_socket_settimeout, nic->socket_settimeout);
    bind_method(impl, MP_QSTR_socket_ioctl, nic->socket_ioctl);

    // Register with network module
    mod_network_register_nic(MP_OBJ_FROM_PTR(nic));

    return MP_OBJ_FROM_PTR(nic);
}

int network_pint_gethostbyname(mp_obj_t nic_in, const char *name, mp_uint_t len, uint8_t *out_ip) {
    debug_printf("gethostbyname(%s)\n", name);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(nic_in);

    mp_obj_t mp_name = mp_obj_new_str(name, len);
    mp_obj_t result = call_method(nic->gethostbyname, 1, mp_name);

    if (result != mp_const_none) {
        if(mp_obj_is_exact_type(result, &mp_type_tuple)) {
            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(result, mp_obj_tuple_t);
            out_ip[0] = mp_obj_get_int(tuple->items[0]);
            out_ip[1] = mp_obj_get_int(tuple->items[1]);
            out_ip[2] = mp_obj_get_int(tuple->items[2]);
            out_ip[3] = mp_obj_get_int(tuple->items[3]);
            return 0;
        }
    }

    return -1;
}

void network_pint_deinit(void) {
    // TODO: ????
}

int network_pint_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    debug_printf("socket_socket(%d %d %d)\n", socket->domain, socket->type, socket->proto);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t result = call_method(nic->socket_socket);
    if (result != mp_const_none) {
        socket->_private = (void *)result;
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
}

void network_pint_socket_close(mod_network_socket_obj_t *socket) {
    debug_printf("socket_close(%d)\n", socket->fileno);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    call_method(nic->socket_close, 1, (mp_obj_t)socket->_private);
};

mp_obj_t ip_to_tuple(byte *ip) {
    mp_obj_t _ip_tuple[4];
    _ip_tuple[0] = mp_obj_new_int(ip[0]);
    _ip_tuple[1] = mp_obj_new_int(ip[1]);
    _ip_tuple[2] = mp_obj_new_int(ip[2]);
    _ip_tuple[3] = mp_obj_new_int(ip[3]);
    return mp_obj_new_tuple(4, _ip_tuple);
}

int network_pint_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_bind(%d, %d)\n", socket->fileno, port);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _port = mp_obj_new_int(port);
    mp_obj_t _ip = ip_to_tuple(ip);

    mp_obj_t result = call_method(nic->socket_bind, 3, (mp_obj_t)socket->_private, _ip, _port);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
};

int network_pint_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    debug_printf("socket_listen(%d, %d)\n", socket->fileno, backlog);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _backlog = mp_obj_new_int(backlog);

    mp_obj_t result = call_method(nic->socket_listen, 3, (mp_obj_t)socket->_private, _backlog);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
};

int network_pint_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    debug_printf("socket_accept(%d)\n", socket->fileno);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _port = mp_obj_new_int(*port);
    mp_obj_t _ip = ip_to_tuple(ip);

    mp_obj_t result = call_method(nic->socket_accept, 3, (mp_obj_t)socket->_private, (mp_obj_t)socket2->_private, _ip, _port);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
};

int network_pint_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_connect(%d)\n", socket->fileno);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _port = mp_obj_new_int(port);
    mp_obj_t _ip = ip_to_tuple(ip);

    mp_obj_t result = call_method(nic->socket_connect, 3, (mp_obj_t)socket->_private, _ip, _port);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
};

mp_uint_t network_pint_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    debug_printf("socket_send(%d, -, %d)\n", socket->fileno, len);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _buf = mp_obj_new_bytearray(len, buf);

    mp_obj_t result = call_method(nic->socket_send, 2, (mp_obj_t)socket->_private, _buf);
    if (result != mp_const_none) {
        *_errno = 0;
        return mp_obj_get_int(result);
    }

    *_errno = MP_EINVAL;
    return 0;
};

mp_uint_t network_pint_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    debug_printf("socket_recv(%d, -, %d)\n", socket->fileno, len);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _buf = mp_obj_new_bytearray_by_ref(len, buf);

    mp_obj_t result = call_method(nic->socket_recv, 2, (mp_obj_t)socket->_private, _buf);
    if (result != mp_const_none) {
        *_errno = 0;
        return mp_obj_get_int(result);
    }

    *_errno = MP_EINVAL;
    return 0;
};

mp_uint_t network_pint_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_sendto(%d)\n", socket->fileno);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _buf = mp_obj_new_bytearray(len, buf);
    mp_obj_t _port = mp_obj_new_int(port);
    mp_obj_t _ip = ip_to_tuple(ip);

    mp_obj_t result = call_method(nic->socket_sendto, 4, (mp_obj_t)socket->_private, _buf, _ip, _port);
    if (result != mp_const_none) {
        *_errno = 0;
        return mp_obj_get_int(result);
    }

    *_errno = MP_EINVAL;
    return 0;
};

mp_uint_t network_pint_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    debug_printf("socket_recvfrom(%d)\n", socket->fileno);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _buf = mp_obj_new_bytearray_by_ref(len, buf);
    mp_obj_t _port = mp_obj_new_int(*port);
    mp_obj_t _ip = ip_to_tuple(ip);

    mp_obj_t result = call_method(nic->socket_recvfrom, 4, (mp_obj_t)socket->_private, _buf, _ip, _port);
    if (result != mp_const_none) {
        *_errno = 0;
        return mp_obj_get_int(result);
    }

    *_errno = MP_EINVAL;
    return 0;
};

int network_pint_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    debug_printf("socket_setsockopt(%d, %d)\n", socket->fileno, opt);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _optval = mp_obj_new_bytearray(optlen, optval);
    mp_obj_t _optkey = mp_obj_new_int(opt);
    mp_obj_t _optlvl = mp_obj_new_int(level);

    mp_obj_t result = call_method(nic->socket_setsockopt, 4, (mp_obj_t)socket->_private, _optlvl, _optkey, _optval);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
};

int network_pint_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    debug_printf("socket_settimeout(%d, %d)\n", socket->fileno, timeout_ms);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _timeout_ms = mp_obj_new_int(timeout_ms);

    mp_obj_t result = call_method(nic->socket_settimeout, 2, (mp_obj_t)socket->_private, _timeout_ms);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }

    *_errno = MP_EINVAL;
    return -1;
};

int network_pint_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    debug_printf("socket_ioctl(%d, %d, %d)\n", socket->fileno, request, arg);

    _pint_obj_t *nic = (_pint_obj_t *)MP_OBJ_TO_PTR(socket->nic);

    mp_obj_t _request = mp_obj_new_int(request);
    mp_obj_t _arg = mp_obj_new_int(arg);

    mp_obj_t result = call_method(nic->socket_ioctl, 3, (mp_obj_t)socket->_private, _request, _arg);
    if (result == mp_const_true) {
        *_errno = 0;
        return 0;
    }
    *_errno = MP_EINVAL;
    return -1;
};

}