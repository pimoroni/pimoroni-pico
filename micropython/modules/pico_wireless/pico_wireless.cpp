#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../libraries/pico_wireless/pico_wireless.hpp"

using namespace pimoroni;

PicoWireless *wireless = nullptr;


extern "C" {
#include "pico_wireless.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as picowireless is not initialised. Call picowireless.init() first."

static void mp_obj_to_string(const mp_obj_t &obj, std::string &string_out) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        string_out = std::string((const char*)str);
    }
    else if(mp_obj_is_float(obj))
        mp_raise_TypeError("can't convert 'float' object to str implicitly");
    if(mp_obj_is_int(obj))
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    if(mp_obj_is_bool(obj))
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    else
        mp_raise_TypeError("can't convert object to str implicitly");
}

mp_obj_t picowireless_init() {
    if(wireless == nullptr)
        wireless = new PicoWireless();
    wireless->init();
    return mp_const_none;
}

mp_obj_t picowireless_get_network_data(mp_obj_t ip_out, mp_obj_t mask_out, mp_obj_t gwip_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_remote_data(mp_obj_t sock, mp_obj_t ip_out, mp_obj_t port_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_network(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_passphrase };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string ssid;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        return mp_obj_new_int(wireless->wifi_set_network(ssid));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_passphrase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_passphrase };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_passphrase, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string ssid, passphrase;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        mp_obj_to_string(args[ARG_passphrase].u_obj, passphrase);
        return mp_obj_new_int(wireless->wifi_set_passphrase(ssid, passphrase));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_key(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_key_idx, ARG_passphrase };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_key_idx, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_passphrase, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t key_idx = args[ARG_key_idx].u_int;

        std::string ssid, passphrase;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        mp_obj_to_string(args[ARG_passphrase].u_obj, passphrase);
        return mp_obj_new_int(wireless->wifi_set_key(ssid, key_idx, passphrase));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_valid_params, ARG_local_ip, ARG_gateway, ARG_subnet };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_valid_params, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_local_ip, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_gateway, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_subnet, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t valid_params = args[ARG_valid_params].u_int;
        uint32_t local_ip = args[ARG_local_ip].u_int;
        uint32_t gateway = args[ARG_gateway].u_int;
        uint32_t subnet = args[ARG_subnet].u_int;
        wireless->config(valid_params, local_ip, gateway, subnet);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_set_dns(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_valid_params, ARG_dns_server1, ARG_dns_server2 };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_valid_params, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_dns_server1, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_dns_server2, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t valid_params = args[ARG_valid_params].u_int;
        uint32_t dns_server1 = args[ARG_dns_server1].u_int;
        uint32_t dns_server2 = args[ARG_dns_server2].u_int;
        wireless->set_dns(valid_params, dns_server1, dns_server2);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_set_hostname(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_hostname };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_hostname, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string hostname;
        mp_obj_to_string(args[ARG_hostname].u_obj, hostname);
        wireless->set_hostname(hostname);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_disconnect() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->disconnect());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_connection_status() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_connection_status());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_mac_address() {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_ip_address(mp_obj_t ip_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_subnet_mask(mp_obj_t mask_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_gateway_ip(mp_obj_t ip_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_ssid() {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_bssid() {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_rssi() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_current_rssi());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_encryption_type() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_current_encryption_type());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);    
    
    return mp_const_none;
}

mp_obj_t picowireless_start_scan_networks() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->start_scan_networks());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);    
    
    return mp_const_none;
}

mp_obj_t picowireless_get_scan_networks() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_scan_networks());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);    
    
    return mp_const_none;
}

mp_obj_t picowireless_get_ssid_networks(mp_obj_t network_item) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_enc_type_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_network_item };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_network_item, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t network_item = args[ARG_network_item].u_int;
        return mp_obj_new_int(wireless->get_enc_type_networks(network_item));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_bssid_networks(mp_obj_t network_item, mp_obj_t bssid_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_channel_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_network_item };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_network_item, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t network_item = args[ARG_network_item].u_int;
        return mp_obj_new_int(wireless->get_channel_networks(network_item));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_rssi_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_network_item };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_network_item, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t network_item = args[ARG_network_item].u_int;
        return mp_obj_new_int(wireless->get_rssi_networks(network_item));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_req_host_by_name(mp_obj_t hostname) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_host_by_name(mp_uint_t n_args, const mp_obj_t *args) {
//mp_obj_t picowireless_get_host_by_name(mp_obj_t ip_out) {
//mp_obj_t picowireless_get_host_by_name(mp_obj_t hostname, mp_obj_t ip_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_fw_version() {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_time() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_time());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_set_power_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_mode };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t mode = args[ARG_mode].u_int;
        wireless->set_power_mode(mode);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_ap_network(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_channel };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_channel, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string ssid;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);

        uint8_t channel = args[ARG_channel].u_int;        
        return mp_obj_new_int(wireless->wifi_set_ap_network(ssid, channel));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_wifi_set_ap_passphrase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ssid, ARG_passphrase, ARG_channel };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_passphrase, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_channel, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string ssid, passphrase;
        mp_obj_to_string(args[ARG_ssid].u_obj, ssid);
        mp_obj_to_string(args[ARG_passphrase].u_obj, passphrase);

        uint8_t channel = args[ARG_channel].u_int;        
        return mp_obj_new_int(wireless->wifi_set_ap_passphrase(ssid, passphrase, channel));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_ping(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_ip_address, ARG_ttl };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ip_address, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_ttl, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint32_t ip_address = args[ARG_ip_address].u_int;
        uint8_t ttl = args[ARG_ttl].u_int;
        return mp_obj_new_int(wireless->ping(ip_address, ttl));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_debug(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_on };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_on, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t on = args[ARG_on].u_int;
        wireless->debug(on);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_temperature() {
    if(wireless != nullptr)
        return mp_obj_new_float(wireless->get_temperature());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);    
    
    return mp_const_none;
}

mp_obj_t picowireless_pin_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_pin, ARG_mode };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_esp_pin, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t pin = args[ARG_pin].u_int;
        uint8_t mode = args[ARG_mode].u_int;
        wireless->pin_mode(pin, mode);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_digital_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_pin, ARG_value };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_esp_pin, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t pin = args[ARG_pin].u_int;
        uint8_t value = args[ARG_value].u_int;
        wireless->digital_write(pin, value);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_analog_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_pin, ARG_value };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_esp_pin, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t pin = args[ARG_pin].u_int;
        uint8_t value = args[ARG_value].u_int;
        wireless->analog_write(pin, value);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_server_start(mp_uint_t n_args, const mp_obj_t *args) {
//mp_obj_t picowireless_start_server(mp_obj_t port, mp_obj_t sock, mp_obj_t protocol_mode/*=TCP_MODE*/) {
//mp_obj_t picowireless_start_server(mp_uint_t n_args, const mp_obj_t *args) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_client_start(mp_uint_t n_args, const mp_obj_t *args) {
//(mp_obj_t ip_address, mp_obj_t port, mp_obj_t sock, mp_obj_t protocol_mode/*=TCP_MODE*/)
//(mp_obj_t host, mp_obj_t ip_address, mp_obj_t port, mp_obj_t sock, mp_obj_t protocol_mode/*=TCP_MODE*/) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_client_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        wireless->stop_client(sock);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_server_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        return mp_obj_new_int(wireless->get_server_state(sock));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_client_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        return mp_obj_new_int(wireless->get_client_state(sock));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_avail_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        return mp_obj_new_int(wireless->avail_data(sock));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_avail_server(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        return mp_obj_new_int(wireless->avail_server(sock));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_data(mp_obj_t sock, mp_obj_t data_out, mp_obj_t peek) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_data_buf(mp_obj_t sock, mp_obj_t data_out, mp_obj_t data_len_out) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_insert_data_buf(mp_obj_t sock, mp_obj_t data_in, mp_obj_t len) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_send_udp_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        return mp_obj_new_bool(wireless->send_udp_data(sock));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_send_data(mp_obj_t sock, mp_obj_t data_in, mp_obj_t len) {
    if(wireless != nullptr)
        //TODO implement
        mp_raise_NotImplementedError("Please avoid using this function for now");
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_check_data_sent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)  {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t sock = args[ARG_sock].u_int;
        return mp_obj_new_int(wireless->check_data_sent(sock));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_socket() {
    if(wireless != nullptr)
        return mp_obj_new_int(wireless->get_socket());
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}
}