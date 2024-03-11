#include "pint.h"

MP_DEFINE_CONST_FUN_OBJ_1(network_pint__del__obj, network_pint__del__);

static const mod_network_nic_protocol_t mod_network_nic_protocol_pint = {
    .gethostbyname = network_pint_gethostbyname,
    .deinit = network_pint_deinit,
    .socket = network_pint_socket_socket,
    .close = network_pint_socket_close,
    .bind = network_pint_socket_bind,
    .listen = network_pint_socket_listen,
    .accept = network_pint_socket_accept,
    .connect = network_pint_socket_connect,
    .send = network_pint_socket_send,
    .recv = network_pint_socket_recv,
    .sendto = network_pint_socket_sendto,
    .recvfrom = network_pint_socket_recvfrom,
    .setsockopt = network_pint_socket_setsockopt,
    .settimeout = network_pint_socket_settimeout,
    .ioctl = network_pint_socket_ioctl,
};

static const mp_rom_map_elem_t pint_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&network_pint__del__obj) },
};

static MP_DEFINE_CONST_DICT(pint_locals_dict, pint_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_pint,
    MP_QSTR_pint,
    MP_TYPE_FLAG_NONE,
    make_new, network_pint_make_new,
    locals_dict, &pint_locals_dict,
    protocol, &mod_network_nic_protocol_pint
);

#ifdef NETWORK_PINT_MODULE
static const mp_map_elem_t pint_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pint) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PINT), (mp_obj_t)&mod_network_nic_type_pint },
};

static MP_DEFINE_CONST_DICT(mp_module_pint_globals, pint_globals_table);

const mp_obj_module_t pint_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_pint_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pint, pint_user_cmodule);
#endif