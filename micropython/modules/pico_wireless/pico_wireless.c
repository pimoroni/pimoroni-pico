#include "pico_wireless.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// picowireless Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_init_obj, picowireless_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_network_data_obj, picowireless_get_network_data);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_remote_data_obj, 1, picowireless_get_remote_data);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_network_obj, 1, picowireless_wifi_set_network);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_passphrase_obj, 2, picowireless_wifi_set_passphrase);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_key_obj, 3, picowireless_wifi_set_key);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_config_obj, 4, picowireless_config);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_set_dns_obj, 1, picowireless_set_dns);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_set_hostname_obj, 1, picowireless_set_hostname);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_disconnect_obj, picowireless_disconnect);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_connection_status_obj, picowireless_get_connection_status);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_mac_address_obj, picowireless_get_mac_address);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_ip_address_obj, picowireless_get_ip_address);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_subnet_mask_obj, picowireless_get_subnet_mask);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_gateway_ip_obj, picowireless_get_gateway_ip);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_current_ssid_obj, picowireless_get_current_ssid);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_current_bssid_obj, picowireless_get_current_bssid);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_current_rssi_obj, picowireless_get_current_rssi);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_current_encryption_type_obj, picowireless_get_current_encryption_type);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_start_scan_networks_obj, picowireless_start_scan_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_scan_networks_obj, picowireless_get_scan_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_ssid_networks_obj, 1, picowireless_get_ssid_networks);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_enc_type_networks_obj, 1, picowireless_get_enc_type_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_bssid_networks_obj, 1, picowireless_get_bssid_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_channel_networks_obj, 1, picowireless_get_channel_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_rssi_networks_obj, 1, picowireless_get_rssi_networks);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_req_host_by_name_obj, 1, picowireless_req_host_by_name);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_host_by_name_obj, 0, picowireless_get_host_by_name);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_fw_version_obj, picowireless_get_fw_version);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_time_obj, picowireless_get_time);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_set_power_mode_obj, 1, picowireless_set_power_mode);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_ap_network_obj, 2, picowireless_wifi_set_ap_network);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_ap_passphrase_obj, 3, picowireless_wifi_set_ap_passphrase);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_ping_obj, 2, picowireless_ping);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_debug_obj, 1, picowireless_debug);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_temperature_obj, picowireless_get_temperature);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_pin_mode_obj, 2, picowireless_pin_mode);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_digital_write_obj, 2, picowireless_digital_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_analog_write_obj, 2, picowireless_analog_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_digital_read_obj, 1, picowireless_digital_read);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_analog_read_obj, 1, picowireless_analog_read);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_server_start_obj, 3, picowireless_server_start);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_client_start_obj, 4, picowireless_client_start);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_client_stop_obj, 1, picowireless_client_stop);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_server_state_obj, 1, picowireless_get_server_state);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_client_state_obj, 1, picowireless_get_client_state);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_avail_data_obj, 1, picowireless_avail_data);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_avail_server_obj, 1, picowireless_avail_server);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_data_obj, 2, picowireless_get_data);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_get_data_buf_obj, 1, picowireless_get_data_buf);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_insert_data_buf_obj, 2, picowireless_insert_data_buf);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_send_udp_data_obj, 1, picowireless_send_udp_data);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_send_data_obj, 2, picowireless_send_data);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_check_data_sent_obj, 1, picowireless_check_data_sent);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_get_socket_obj, picowireless_get_socket);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_ent_identity_obj, 1, picowireless_wifi_set_ent_identity);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_ent_username_obj, 1, picowireless_wifi_set_ent_username);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_wifi_set_ent_password_obj, 1, picowireless_wifi_set_ent_password);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_wifi_set_ent_enable_obj, picowireless_wifi_set_ent_enable);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picowireless_set_led_obj, 3, picowireless_set_led);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_is_pressed_obj, picowireless_is_pressed);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picowireless_is_sdcard_detected_obj, picowireless_is_sdcard_detected);


/***** Globals Table *****/
STATIC const mp_map_elem_t picowireless_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picowireless) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picowireless_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_network_data), MP_ROM_PTR(&picowireless_get_network_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_remote_data), MP_ROM_PTR(&picowireless_get_remote_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_network), MP_ROM_PTR(&picowireless_wifi_set_network_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_passphrase), MP_ROM_PTR(&picowireless_wifi_set_passphrase_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_key), MP_ROM_PTR(&picowireless_wifi_set_key_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&picowireless_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_dns), MP_ROM_PTR(&picowireless_set_dns_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hostname), MP_ROM_PTR(&picowireless_set_hostname_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&picowireless_disconnect_obj) }
    ,
    { MP_ROM_QSTR(MP_QSTR_get_connection_status), MP_ROM_PTR(&picowireless_get_connection_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_mac_address), MP_ROM_PTR(&picowireless_get_mac_address_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_ip_address), MP_ROM_PTR(&picowireless_get_ip_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_subnet_mask), MP_ROM_PTR(&picowireless_get_subnet_mask_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_gateway_ip), MP_ROM_PTR(&picowireless_get_gateway_ip_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_current_ssid), MP_ROM_PTR(&picowireless_get_current_ssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_current_bssid), MP_ROM_PTR(&picowireless_get_current_bssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_current_rssi), MP_ROM_PTR(&picowireless_get_current_rssi_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_current_encryption_type), MP_ROM_PTR(&picowireless_get_current_encryption_type_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_scan_networks), MP_ROM_PTR(&picowireless_start_scan_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_scan_networks), MP_ROM_PTR(&picowireless_get_scan_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_ssid_networks), MP_ROM_PTR(&picowireless_get_ssid_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_enc_type_networks), MP_ROM_PTR(&picowireless_get_enc_type_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_bssid_networks), MP_ROM_PTR(&picowireless_get_bssid_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_channel_networks), MP_ROM_PTR(&picowireless_get_channel_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_rssi_networks), MP_ROM_PTR(&picowireless_get_rssi_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_req_host_by_name), MP_ROM_PTR(&picowireless_req_host_by_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_host_by_name), MP_ROM_PTR(&picowireless_get_host_by_name_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_fw_version), MP_ROM_PTR(&picowireless_get_fw_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_time), MP_ROM_PTR(&picowireless_get_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_power_mode), MP_ROM_PTR(&picowireless_set_power_mode_obj) },

    { MP_ROM_QSTR(MP_QSTR_wifi_set_ap_network), MP_ROM_PTR(&picowireless_wifi_set_ap_network_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_ap_passphrase), MP_ROM_PTR(&picowireless_wifi_set_ap_passphrase_obj) },
    { MP_ROM_QSTR(MP_QSTR_ping), MP_ROM_PTR(&picowireless_ping_obj) },

    { MP_ROM_QSTR(MP_QSTR_debug), MP_ROM_PTR(&picowireless_debug_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_temperature), MP_ROM_PTR(&picowireless_get_temperature_obj) },
    { MP_ROM_QSTR(MP_QSTR_pin_mode), MP_ROM_PTR(&picowireless_pin_mode_obj) },

    { MP_ROM_QSTR(MP_QSTR_digital_write), MP_ROM_PTR(&picowireless_digital_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_analog_write), MP_ROM_PTR(&picowireless_analog_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_digital_read), MP_ROM_PTR(&picowireless_digital_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_analog_read), MP_ROM_PTR(&picowireless_analog_read_obj) },

    { MP_ROM_QSTR(MP_QSTR_server_start), MP_ROM_PTR(&picowireless_server_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_client_start), MP_ROM_PTR(&picowireless_client_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_client_stop), MP_ROM_PTR(&picowireless_client_stop_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_server_state), MP_ROM_PTR(&picowireless_get_server_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_client_state), MP_ROM_PTR(&picowireless_get_client_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_avail_data), MP_ROM_PTR(&picowireless_avail_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_avail_server), MP_ROM_PTR(&picowireless_avail_server_obj) },

    { MP_ROM_QSTR(MP_QSTR_get_data), MP_ROM_PTR(&picowireless_get_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_data_buf), MP_ROM_PTR(&picowireless_get_data_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_insert_data_buf), MP_ROM_PTR(&picowireless_insert_data_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_udp_data), MP_ROM_PTR(&picowireless_send_udp_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_data), MP_ROM_PTR(&picowireless_send_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_check_data_sent), MP_ROM_PTR(&picowireless_check_data_sent_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_socket), MP_ROM_PTR(&picowireless_get_socket_obj) },

    { MP_ROM_QSTR(MP_QSTR_wifi_set_ent_identity), MP_ROM_PTR(&picowireless_wifi_set_ent_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_ent_username), MP_ROM_PTR(&picowireless_wifi_set_ent_username_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_ent_password), MP_ROM_PTR(&picowireless_wifi_set_ent_password_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_ent_enable), MP_ROM_PTR(&picowireless_wifi_set_ent_enable_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_set_led), MP_ROM_PTR(&picowireless_set_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pressed), MP_ROM_PTR(&picowireless_is_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_sdcard_detected), MP_ROM_PTR(&picowireless_is_sdcard_detected_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picowireless_globals, picowireless_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picowireless_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picowireless_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_picowireless, picowireless_user_cmodule, MODULE_PICO_WIRELESS_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_picowireless, picowireless_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////