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
    else if(mp_obj_is_int(obj))
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    else if(mp_obj_is_bool(obj))
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    else
        mp_raise_TypeError("can't convert object to str implicitly");
}

uint32_t mp_obj_to_ip(mp_obj_t obj) {
    uint len;
    mp_obj_t *items;
    mp_obj_tuple_get(obj, &len, &items);
    uint a = mp_obj_get_int(items[0]) & 0xff;
    uint b = mp_obj_get_int(items[1]) & 0xff;
    uint c = mp_obj_get_int(items[2]) & 0xff;
    uint d = mp_obj_get_int(items[3]) & 0xff;
    return (d << 24) | (c << 16) | (b << 8) | a;
}

mp_obj_t mp_ip_to_obj(IPAddress ip) {
    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(ip[0]);
    tuple[1] = mp_obj_new_int(ip[1]);
    tuple[2] = mp_obj_new_int(ip[2]);
    tuple[3] = mp_obj_new_int(ip[3]);
    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t picowireless_init() {
    if(wireless == nullptr)
        wireless = new PicoWireless();
    wireless->init();
    return mp_const_none;
}

mp_obj_t picowireless_get_network_data() {
    if(wireless != nullptr) {
        uint8_t *ip = nullptr;
        uint8_t *mask = nullptr;
        uint8_t *gwip = nullptr;
        wireless->get_network_data(ip, mask, gwip);

        mp_obj_t tuple[3];
        tuple[0] = mp_obj_new_bytes(ip, WL_IPV4_LENGTH);
        tuple[1] = mp_obj_new_bytes(mask, WL_IPV4_LENGTH);
        tuple[2] = mp_obj_new_bytes(gwip, WL_IPV4_LENGTH);
        return mp_obj_new_tuple(3, tuple);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_remote_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t *ip = nullptr;
        uint8_t *port = nullptr;
        wireless->get_remote_data(args[ARG_sock].u_int, ip, port);

        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_bytes(ip, WL_IPV4_LENGTH);
        tuple[1] = mp_obj_new_int((uint16_t)port[0] << 8 | (uint16_t)port[1]); //TODO verify size and ordering of port
        return mp_obj_new_tuple(2, tuple);
    }
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
            { MP_QSTR_local_ip, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_gateway, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_subnet, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t valid_params = args[ARG_valid_params].u_int;
        uint32_t local_ip = mp_obj_to_ip(args[ARG_local_ip].u_obj);
        uint32_t gateway = mp_obj_to_ip(args[ARG_gateway].u_obj);
        uint32_t subnet = mp_obj_to_ip(args[ARG_subnet].u_obj);
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
            { MP_QSTR_dns_server1, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_dns_server2, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t valid_params = args[ARG_valid_params].u_int;
        uint32_t dns_server1 = mp_obj_to_ip(args[ARG_dns_server1].u_obj);
        uint32_t dns_server2 = mp_obj_to_ip(args[ARG_dns_server2].u_obj);
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
    if(wireless != nullptr) {
        uint8_t* mac = wireless->get_mac_address();
        return mp_obj_new_bytes(mac, WL_MAC_ADDR_LENGTH);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_ip_address() {
    if(wireless != nullptr) {
        IPAddress ip;
        wireless->get_ip_address(ip);
        return mp_ip_to_obj(ip);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_subnet_mask() {
    if(wireless != nullptr) {
        IPAddress mask;
        wireless->get_subnet_mask(mask);
        return mp_ip_to_obj(mask);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_gateway_ip() {
    if(wireless != nullptr) {
        IPAddress mask;
        wireless->get_gateway_ip(mask);
        return mp_ip_to_obj(mask);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_ssid() {
    if(wireless != nullptr) {
        std::string ssid = wireless->get_current_ssid();
        return mp_obj_new_str(ssid.c_str(), ssid.length());
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_current_bssid() {
    if(wireless != nullptr) {
        uint8_t* bssid = wireless->get_current_bssid();
        return mp_obj_new_bytes(bssid, WL_MAC_ADDR_LENGTH);
    }
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

mp_obj_t picowireless_get_ssid_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_network_item };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_network_item, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t network_item = args[ARG_network_item].u_int;
        const char* ssid = wireless->get_ssid_networks(network_item);
        if(ssid != nullptr) {
            std::string str_ssid(ssid, WL_SSID_MAX_LENGTH);
            return mp_obj_new_str(str_ssid.c_str(), str_ssid.length());
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_enc_type_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_get_bssid_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_network_item };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_network_item, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t network_item = args[ARG_network_item].u_int;
        uint8_t* bssid = nullptr;
        wireless->get_bssid_networks(network_item, bssid);
        if(bssid != nullptr) {
            return mp_obj_new_bytes(bssid, WL_MAC_ADDR_LENGTH);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_channel_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_get_rssi_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_req_host_by_name(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_hostname };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_hostname, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        std::string hostname;
        mp_obj_to_string(args[ARG_hostname].u_obj, hostname);

        return mp_obj_new_bool(wireless->req_host_by_name(hostname));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_false;
}

mp_obj_t picowireless_get_host_by_name(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        if(n_args == 0) {
            IPAddress ip;
            if(wireless->get_host_by_name(ip)) {
                return mp_ip_to_obj(ip);
            }
        }
        else {
            enum { ARG_hostname };
            static const mp_arg_t allowed_args[] = {
                { MP_QSTR_hostname, MP_ARG_REQUIRED | MP_ARG_OBJ },
            };

            mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
            mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

            std::string hostname;
            mp_obj_to_string(args[ARG_hostname].u_obj, hostname);

            IPAddress ip;
            if(wireless->get_host_by_name(hostname, ip)) {
                return mp_ip_to_obj(ip);
            }
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_fw_version() {
    if(wireless != nullptr) {
        const char* fw_ver = wireless->get_fw_version();
        std::string str_fw_ver(fw_ver, WL_FW_VER_LENGTH);
        return mp_obj_new_str(str_fw_ver.c_str(), str_fw_ver.length());
    }
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

mp_obj_t picowireless_ping(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_ip_address, ARG_ttl };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_ip_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_ttl, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint32_t ip_address = mp_obj_to_ip(args[ARG_ip_address].u_obj);
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

mp_obj_t picowireless_server_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        if(n_args == 3) {
            enum { ARG_port, ARG_sock, ARG_protocol_mode };
            static const mp_arg_t allowed_args[] = {
                { MP_QSTR_prt, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_protocol_mode, MP_ARG_REQUIRED | MP_ARG_INT },
            };

            mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
            mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

            uint16_t port = args[ARG_port].u_int;
            uint8_t sock = args[ARG_sock].u_int;
            uint8_t protocol_mode = args[ARG_protocol_mode].u_int;
            wireless->start_server(port, sock, protocol_mode);
        }
        else {
            enum { ARG_ip_address, ARG_port, ARG_sock, ARG_protocol_mode };
            static const mp_arg_t allowed_args[] = {
                { MP_QSTR_ip_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
                { MP_QSTR_prt, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_protocol_mode, MP_ARG_REQUIRED | MP_ARG_INT },
            };

            mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
            mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

            uint32_t ip_address = mp_obj_to_ip(args[ARG_ip_address].u_obj);
            uint16_t port = args[ARG_port].u_int;
            uint8_t sock = args[ARG_sock].u_int;
            uint8_t protocol_mode = args[ARG_protocol_mode].u_int;
            wireless->start_server(ip_address, port, sock, protocol_mode);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_client_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        if(n_args == 4) {
            enum { ARG_ip_address, ARG_port, ARG_sock, ARG_protocol_mode };
            static const mp_arg_t allowed_args[] = {
                { MP_QSTR_ip_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
                { MP_QSTR_prt, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_protocol_mode, MP_ARG_REQUIRED | MP_ARG_INT },
            };

            mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
            mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


            uint32_t ip_address = mp_obj_to_ip(args[ARG_ip_address].u_obj);
            uint16_t port = args[ARG_port].u_int;
            uint8_t sock = args[ARG_sock].u_int;
            uint8_t protocol_mode = args[ARG_protocol_mode].u_int;
            wireless->start_client(ip_address, port, sock, protocol_mode);
        }
        else {
            enum { ARG_hostname, ARG_ip_address, ARG_port, ARG_sock, ARG_protocol_mode };
            static const mp_arg_t allowed_args[] = {
                { MP_QSTR_hostname, MP_ARG_REQUIRED | MP_ARG_OBJ },
                { MP_QSTR_ip_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
                { MP_QSTR_prt, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
                { MP_QSTR_protocol_mode, MP_ARG_REQUIRED | MP_ARG_INT },
            };

            mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
            mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

            std::string hostname;
            mp_obj_to_string(args[ARG_hostname].u_obj, hostname);

            uint32_t ip_address = mp_obj_to_ip(args[ARG_ip_address].u_obj);
            uint16_t port = args[ARG_port].u_int;
            uint8_t sock = args[ARG_sock].u_int;
            uint8_t protocol_mode = args[ARG_protocol_mode].u_int;
            wireless->start_client(hostname, ip_address, port, sock, protocol_mode);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_client_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_get_server_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_get_client_state(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_avail_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_avail_server(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_get_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock, ARG_peek };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_peek, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t *data = nullptr;
        if(wireless->get_data(args[ARG_sock].u_int, data, args[ARG_peek].u_int)) {
            return mp_obj_new_int(*data);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_get_data_buf(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t *data = (uint8_t *)malloc(512);
        uint16_t data_len = 512;
        if(wireless->get_data_buf(args[ARG_sock].u_int, data, &data_len)) {
            mp_obj_t response = mp_obj_new_bytes(data, data_len);
            free(data);
            return response;
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_insert_data_buf(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock, ARG_data};
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

        return mp_obj_new_bool(wireless->insert_data_buf(args[ARG_sock].u_int, (uint8_t *)bufinfo.buf, bufinfo.len));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_send_udp_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

mp_obj_t picowireless_send_data(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock, ARG_data};
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

        return mp_obj_new_int(wireless->send_data(args[ARG_sock].u_int, (uint8_t *)bufinfo.buf, bufinfo.len));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_const_none;
}

mp_obj_t picowireless_check_data_sent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_sock };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_sock, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        return mp_obj_new_int(wireless->check_data_sent(args[ARG_sock].u_int));
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

mp_obj_t picowireless_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(wireless != nullptr) {
        enum { ARG_r, ARG_g, ARG_b };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        int r = args[ARG_r].u_int;
        int g = args[ARG_g].u_int;
        int b = args[ARG_b].u_int;

        if(r < 0 || r > 255)
            mp_raise_ValueError("r out of range. Expected 0 to 255");
        else if(g < 0 || g > 255)
            mp_raise_ValueError("g out of range. Expected 0 to 255");
        else if(b < 0 || b > 255)
            mp_raise_ValueError("b out of range. Expected 0 to 255");
        else
            wireless->set_led(r, g, b);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picowireless_is_pressed() {
    bool buttonPressed = false;
    
    if(wireless != nullptr) {
        buttonPressed = wireless->is_pressed(PicoWireless::A);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return buttonPressed ? mp_const_true : mp_const_false;
}
}