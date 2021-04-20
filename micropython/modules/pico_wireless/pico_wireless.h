// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// Declare the functions we'll make available in Python
extern mp_obj_t picowireless_init();
extern mp_obj_t picowireless_get_network_data();
extern mp_obj_t picowireless_get_remote_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_wifi_set_network(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_passphrase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_key(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_set_dns(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_set_hostname(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_disconnect();

extern mp_obj_t picowireless_get_connection_status();
extern mp_obj_t picowireless_get_mac_address();

extern mp_obj_t picowireless_get_ip_address();
extern mp_obj_t picowireless_get_subnet_mask();
extern mp_obj_t picowireless_get_gateway_ip();

extern mp_obj_t picowireless_get_current_ssid();
extern mp_obj_t picowireless_get_current_bssid();
extern mp_obj_t picowireless_get_current_rssi();
extern mp_obj_t picowireless_get_current_encryption_type();

extern mp_obj_t picowireless_start_scan_networks();
extern mp_obj_t picowireless_get_scan_networks();
extern mp_obj_t picowireless_get_ssid_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_get_enc_type_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_bssid_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_channel_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_rssi_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_req_host_by_name(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_host_by_name(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_get_fw_version();
extern mp_obj_t picowireless_get_time();
extern mp_obj_t picowireless_set_power_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_wifi_set_ap_network(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_ap_passphrase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_ping(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_debug(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_temperature();
extern mp_obj_t picowireless_pin_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_digital_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_analog_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_digital_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_analog_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_server_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_client_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_client_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_get_server_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_client_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_avail_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_avail_server(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_get_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_data_buf(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_insert_data_buf(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_send_udp_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picowireless_send_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_check_data_sent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_get_socket();

extern mp_obj_t picowireless_wifi_set_ent_identity(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_ent_username(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_ent_password(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_wifi_set_ent_enable();

extern mp_obj_t picowireless_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picowireless_is_pressed();
extern mp_obj_t picowireless_is_sdcard_detected();