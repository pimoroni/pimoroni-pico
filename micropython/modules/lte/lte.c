#include "lte.h"

static const mod_network_nic_protocol_t mod_network_nic_protocol_lte = {
    .gethostbyname = network_lte_gethostbyname,
    .deinit = network_lte_deinit,
    .socket = network_lte_socket_socket,
    .close = network_lte_socket_close,
    .bind = network_lte_socket_bind,
    .listen = network_lte_socket_listen,
    .accept = network_lte_socket_accept,
    .connect = network_lte_socket_connect,
    .send = network_lte_socket_send,
    .recv = network_lte_socket_recv,
    .sendto = network_lte_socket_sendto,
    .recvfrom = network_lte_socket_recvfrom,
    .setsockopt = network_lte_socket_setsockopt,
    .settimeout = network_lte_socket_settimeout,
    .ioctl = network_lte_socket_ioctl,
};

static const mp_rom_map_elem_t lte_locals_dict_table[] = {
};

static MP_DEFINE_CONST_DICT(lte_locals_dict, lte_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_lte,
    MP_QSTR_lte,
    MP_TYPE_FLAG_NONE,
    make_new, network_lte_make_new,
    locals_dict, &lte_locals_dict,
    protocol, &mod_network_nic_protocol_lte
);

#ifdef NETWORK_LTE_MODULE
static const mp_map_elem_t lte_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lte) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LTE), (mp_obj_t)&mod_network_nic_type_lte },
};

static MP_DEFINE_CONST_DICT(mp_module_lte_globals, lte_globals_table);

const mp_obj_module_t lte_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_lte_globals,
};

MP_REGISTER_MODULE(MP_QSTR_lte, lte_user_cmodule);
#endif