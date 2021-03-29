#include "esp32spi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// esp32spi Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_init_obj, esp32spi_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_reset_obj, esp32spi_reset);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_status_obj, esp32spi_status);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_firmware_version_obj, esp32spi_firmware_version);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_MAC_address_obj, esp32spi_MAC_address);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_MAC_address_actual_obj, esp32spi_MAC_address_actual);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_start_scan_networks_obj, esp32spi_start_scan_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_get_scan_networks_obj, esp32spi_get_scan_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_scan_networks_obj, esp32spi_scan_networks);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_wifi_set_network_obj, esp32spi_wifi_set_network);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_wifi_set_passphrase_obj, esp32spi_wifi_set_passphrase);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_wifi_set_entidentity_obj, esp32spi_wifi_set_entidentity);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_wifi_set_entusername_obj, esp32spi_wifi_set_entusername);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_wifi_set_entpassword_obj, esp32spi_wifi_set_entpassword);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_wifi_set_entenable_obj, esp32spi_wifi_set_entenable);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_ssid_obj, esp32spi_ssid);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_bssid_obj, esp32spi_bssid);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_rssi_obj, esp32spi_rssi);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_network_data_obj, esp32spi_network_data);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_ip_address_obj, esp32spi_ip_address);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_is_connected_obj, esp32spi_is_connected);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_ap_listening_obj, esp32spi_ap_listening);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_disconnect_obj, esp32spi_disconnect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_connect_obj, esp32spi_connect);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32spi_connect_AP_obj, esp32spi_connect_AP);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32spi_create_AP_obj, 4, 4, esp32spi_create_AP);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_pretty_ip_obj, esp32spi_pretty_ip);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_unpretty_ip_obj, esp32spi_unpretty_ip);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_get_host_by_name_obj, esp32spi_get_host_by_name);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_ping_obj, esp32spi_ping);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_get_socket_obj, esp32spi_get_socket);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32spi_socket_open_obj, 4, 4, esp32spi_socket_open);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_socket_status_obj, esp32spi_socket_status);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_socket_connected_obj, esp32spi_socket_connected);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32spi_socket_write_obj, esp32spi_socket_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_socket_available_obj, esp32spi_socket_available);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_socket_read_obj, esp32spi_socket_read);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32spi_socket_connect_obj, 4, 4, esp32spi_socket_connect);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_socket_close_obj, esp32spi_socket_close);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32spi_start_server_obj, 4, 4, esp32spi_start_server);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_server_state_obj, esp32spi_server_state);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_esp_debug_obj, esp32spi_esp_debug);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_set_pin_mode_obj, esp32spi_set_pin_mode);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_set_digital_write_obj, esp32spi_set_digital_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_set_analog_write_obj, esp32spi_set_analog_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_set_digital_read_obj, esp32spi_set_digital_read);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32spi_set_analog_read_obj, esp32spi_set_analog_read);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32spi_get_time_obj, esp32spi_get_time);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_set_certificate_obj, esp32spi_set_certificate);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32spi_set_private_key_obj, esp32spi_set_private_key);

/***** Globals Table *****/
STATIC const mp_map_elem_t esp32spi_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp32spi) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&esp32spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&esp32spi_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_status2), MP_ROM_PTR(&esp32spi_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_firmware_version), MP_ROM_PTR(&esp32spi_firmware_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_MAC_address), MP_ROM_PTR(&esp32spi_MAC_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_MAC_address_actual), MP_ROM_PTR(&esp32spi_MAC_address_actual_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_scan_networks), MP_ROM_PTR(&esp32spi_start_scan_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_scan_networks), MP_ROM_PTR(&esp32spi_get_scan_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan_networks), MP_ROM_PTR(&esp32spi_scan_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_network), MP_ROM_PTR(&esp32spi_wifi_set_network_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_passphrase), MP_ROM_PTR(&esp32spi_wifi_set_passphrase_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_entidentity), MP_ROM_PTR(&esp32spi_wifi_set_entidentity_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_entusername), MP_ROM_PTR(&esp32spi_wifi_set_entusername_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_entpassword), MP_ROM_PTR(&esp32spi_wifi_set_entpassword_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_entenable), MP_ROM_PTR(&esp32spi_wifi_set_entenable_obj) },
    { MP_ROM_QSTR(MP_QSTR_ssid), MP_ROM_PTR(&esp32spi_ssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_bssid), MP_ROM_PTR(&esp32spi_bssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_rssi), MP_ROM_PTR(&esp32spi_rssi_obj) },
    { MP_ROM_QSTR(MP_QSTR_network_data), MP_ROM_PTR(&esp32spi_network_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_ip_address), MP_ROM_PTR(&esp32spi_ip_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_connected), MP_ROM_PTR(&esp32spi_is_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ap_listening), MP_ROM_PTR(&esp32spi_ap_listening_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&esp32spi_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect2), MP_ROM_PTR(&esp32spi_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect_AP), MP_ROM_PTR(&esp32spi_connect_AP_obj) },
    { MP_ROM_QSTR(MP_QSTR_create_AP), MP_ROM_PTR(&esp32spi_create_AP_obj) },
    { MP_ROM_QSTR(MP_QSTR_pretty_ip), MP_ROM_PTR(&esp32spi_pretty_ip_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpretty_ip), MP_ROM_PTR(&esp32spi_unpretty_ip_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_host_by_name), MP_ROM_PTR(&esp32spi_get_host_by_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_ping), MP_ROM_PTR(&esp32spi_ping_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_socket), MP_ROM_PTR(&esp32spi_get_socket_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_open), MP_ROM_PTR(&esp32spi_socket_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_status), MP_ROM_PTR(&esp32spi_socket_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_connected), MP_ROM_PTR(&esp32spi_socket_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_write), MP_ROM_PTR(&esp32spi_socket_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_available), MP_ROM_PTR(&esp32spi_socket_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_read), MP_ROM_PTR(&esp32spi_socket_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_connect), MP_ROM_PTR(&esp32spi_socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket_close), MP_ROM_PTR(&esp32spi_socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_server2), MP_ROM_PTR(&esp32spi_start_server_obj) },
    { MP_ROM_QSTR(MP_QSTR_server_state), MP_ROM_PTR(&esp32spi_server_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_esp_debug), MP_ROM_PTR(&esp32spi_esp_debug_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pin_mode), MP_ROM_PTR(&esp32spi_set_pin_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_digital_write), MP_ROM_PTR(&esp32spi_set_digital_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_analog_write), MP_ROM_PTR(&esp32spi_set_analog_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_digital_read), MP_ROM_PTR(&esp32spi_set_digital_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_analog_read), MP_ROM_PTR(&esp32spi_set_analog_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_time), MP_ROM_PTR(&esp32spi_get_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_certificate), MP_ROM_PTR(&esp32spi_set_certificate_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_private_key), MP_ROM_PTR(&esp32spi_set_private_key_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_esp32spi_globals, esp32spi_globals_table);

/***** Module Definition *****/
const mp_obj_module_t esp32spi_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_esp32spi_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_esp32spi, esp32spi_user_cmodule, MODULE_ESP32SPI_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////