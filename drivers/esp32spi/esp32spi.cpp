#include "esp32spi.hpp"

namespace pimoroni {


  enum cmd {
    // 0x10 -> 0x1f
    SET_NET                 = 0x10,
    SET_PASSPHRASE          = 0x11,
    SET_KEY                 = 0x12,
    //NULL
    SET_IP_CONFIG           = 0x14,
    SET_DNS_CONFIG          = 0x15,
    SET_HOSTNAME            = 0x16,
    SET_POWER_MODE          = 0x17,
    SET_AP_NET              = 0x18,
    SET_AP_PASSPHRASE       = 0x19,
    SET_DEBUG               = 0x1a,
    GET_TEMPERATURE         = 0x1b,
    //NULL, NULL, NULL, NULL,

    // 0x20 -> 0x2f
    GET_CONN_STATUS         = 0x20,
    GET_IP_ADDR             = 0x21,
    GET_MAC_ADDR            = 0x22,
    GET_CURR_SSID           = 0x23,
    GET_CURR_BSSID          = 0x24,
    GET_CURR_RSSI           = 0x25,
    GET_CURR_ENCT           = 0x26,
    SCAN_NETWORKS           = 0x27,
    START_SERVER_TCP        = 0x28,
    GET_STATE_TCP           = 0x29,
    DATA_SENT_TCP           = 0x2a,
    AVAIL_DATA_TCP          = 0x2b,
    GET_DATA_TCP            = 0x2c,
    START_CLIENT_TCP        = 0x2d,
    STOP_CLIENT_TCP         = 0x2e,
    GET_CLIENT_STATE_TCP    = 0x2f,

    // 0x30 -> 0x3f
    DISCONNECT              = 0x30,
    //NULL,
    GET_IDX_RSSI            = 0x32,
    GET_IDX_ENCT            = 0x33,
    REQ_HOST_BY_NAME        = 0x34,
    GET_HOST_BY_NAME        = 0x35,
    START_SCAN_NETWORKS     = 0x36,
    GET_FW_VERSION          = 0x37,
    //NULL,
    SEND_DATA_UDP           = 0x39,
    GET_REMOTE_DATA         = 0x3a,
    GET_TIME                = 0x3b,
    GET_IDX_BSSID           = 0x3c,
    GET_IDX_CHANNEL         = 0x3d,
    PING                    = 0x3e,
    GET_SOCKET              = 0x3f,

    // 0x40 -> 0x4f
    SET_CLIENT_CERT         = 0x40,         //NOTE No matching function
    SET_CERT_KEY            = 0x41,         //NOTE No matching function
    //NULL, NULL,
    SEND_DATA_TCP           = 0x44,
    GET_DATABUF_TCP         = 0x45,
    INSERT_DATABUF          = 0x46,
    //NULL, NULL, NULL,
    WPA2_ENT_SET_IDENTITY   = 0x4a,
    WPA2_ENT_SET_USERNAME   = 0x4b,
    WPA2_ENT_SET_PASSWORD   = 0x4c,
    WPA2_ENT_SET_CA_CERT    = 0x4d,         //NOTE Not functional in Nina FW
    WPA2_ENT_SET_CERT_KEY   = 0x4e,         //NOTE Not functional in Nina FW
    WPA2_ENT_ENABLE         = 0x4f,

    // 0x50 -> 0x5f
    SET_PIN_MODE            = 0x50,
    SET_DIGITAL_WRITE       = 0x51,
    SET_ANALOG_WRITE        = 0x52,
    SET_DIGITAL_READ        = 0x53,
    SET_ANALOG_READ         = 0x54,

    SET_WAKE_PIN            = 0x55,
    SET_LIGHT_SLEEP         = 0x56,
    SET_DEEP_SLEEP          = 0x57,
  };

  bool Esp32Spi::init() {
    driver.init();
    driver.reset();

    return true;
  }

  bool Esp32Spi::get_network_data(uint8_t *ip_out, uint8_t *mask_out, uint8_t *gwip_out) {
    SpiDrv::outParam params_out[SpiDrv::PARAM_NUMS_3] = { {0, ip_out},
                                                    {0, mask_out},
                                                    {0, gwip_out} };

    return driver.send_command(GET_IP_ADDR, params_out, SpiDrv::PARAM_NUMS_3);
  }

  bool Esp32Spi::get_remote_data(uint8_t sock, uint8_t *ip_out, uint8_t *port_out) {
    SpiDrv::outParam params_out[SpiDrv::PARAM_NUMS_2] = { {0, ip_out},
                                                    {0, port_out} };

    return driver.send_command(GET_REMOTE_DATA, params_out, SpiDrv::PARAM_NUMS_2);
  }

  int8_t Esp32Spi::wifi_set_network(const std::string ssid) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_NET, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_NET\n");
    }
    return (data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
  }

  int8_t Esp32Spi::wifi_set_passphrase(const std::string ssid, const std::string passphrase) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid),
      SpiDrv::build_param(&passphrase)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_PASSPHRASE, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_PASSPHRASE\n");
    }
    return data;
  }

  int8_t Esp32Spi::wifi_set_key(const std::string ssid, uint8_t key_idx, const std::string key) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid),
      SpiDrv::build_param(&key_idx),
      SpiDrv::build_param(&key)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_KEY, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_KEY\n");
    }
    return data;
  }

  void Esp32Spi::config(uint8_t valid_params, uint32_t local_ip, uint32_t gateway, uint32_t subnet) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&valid_params),
      SpiDrv::build_param(&local_ip),
      SpiDrv::build_param(&gateway),
      SpiDrv::build_param(&subnet)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_IP_CONFIG, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_IP_CONFIG\n");
    }
  }

  void Esp32Spi::set_dns(uint8_t valid_params, uint32_t dns_server1, uint32_t dns_server2) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&valid_params),
      SpiDrv::build_param(&dns_server1),
      SpiDrv::build_param(&dns_server2)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_DNS_CONFIG, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_DNS_CONFIG\n");
    }
  }

  void Esp32Spi::set_hostname(const std::string hostname) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&hostname)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_HOSTNAME, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_HOSTNAME\n");
    }
  }

  int8_t Esp32Spi::disconnect() {
    SpiDrv::inParam params[] = {
      {.type = SpiDrv::PARAM_DUMMY}
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(DISCONNECT, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:DISCONNECT\n");
    }
    return data;
  }

  uint8_t Esp32Spi::get_connection_status() {
    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(GET_CONN_STATUS, nullptr, 0, &data, &data_len)) {
      WARN("Error:GET_CONN_STATUS\n");
    }
    return data;
  }

  uint8_t* Esp32Spi::get_mac_address() {
    SpiDrv::inParam params[] = {
      {.type = SpiDrv::PARAM_DUMMY}
    };

    uint16_t data_len = 0;
    driver.send_command(GET_MAC_ADDR, params, PARAM_COUNT(params), (uint8_t *)&mac, &data_len);
    return mac;
  }

  bool Esp32Spi::get_ip_address(IPAddress &ip_out) {
    if (get_network_data(local_ip, subnet_mask, gateway_ip)) {
      ip_out = local_ip;
      return true;
    }
    return false;
  }

  bool Esp32Spi::get_subnet_mask(IPAddress &mask_out) {
    if (get_network_data(local_ip, subnet_mask, gateway_ip)) {
      mask_out = subnet_mask;
      return true;
    }
    return false;
  }

  bool Esp32Spi::get_gateway_ip(IPAddress &ip_out) {
    if (get_network_data(local_ip, subnet_mask, gateway_ip)) {
      ip_out = gateway_ip;
      return true;
    }
    return false;
  }

  std::string Esp32Spi::get_current_ssid() {
    SpiDrv::inParam params[] = {
      {.type = SpiDrv::PARAM_DUMMY}
    };

    uint16_t data_len = 0;
    memset(ssid, 0x00, sizeof(ssid));
    driver.send_command(GET_CURR_SSID, params, PARAM_COUNT(params), (uint8_t *)&ssid, &data_len);
    return ssid;
  }

  uint8_t* Esp32Spi::get_current_bssid() {
    SpiDrv::inParam params[] = {
      {.type = SpiDrv::PARAM_DUMMY}
    };

    uint16_t data_len = 0;
    memset(bssid, 0x00, sizeof(bssid));
    driver.send_command(GET_CURR_BSSID, params, PARAM_COUNT(params), (uint8_t *)&bssid, &data_len);
    return bssid;
  }

  int32_t Esp32Spi::get_current_rssi() {
    SpiDrv::inParam params[] = {
      {.type = SpiDrv::PARAM_DUMMY}
    };

    uint16_t data_len = 0;
    int32_t rssi = 0;
    driver.send_command(GET_CURR_RSSI, params, PARAM_COUNT(params), (uint8_t *)&rssi, &data_len);
    return rssi;
  }

  uint8_t Esp32Spi::get_current_encryption_type() {
    SpiDrv::inParam params[] = {
      {.type = SpiDrv::PARAM_DUMMY}
    };

    uint16_t data_len = 0;
    uint8_t enc_type = 0;
    driver.send_command(GET_CURR_ENCT, params, PARAM_COUNT(params), &enc_type, &data_len);
    return enc_type;
  }

  int8_t Esp32Spi::start_scan_networks() {
    /*uint8_t data = 0;
    uint16_t data_len = 0;
    driver.send_command(START_SCAN_NETWORKS, &data, &data_len, false);
    return ((int8_t)data == WL_FAILURE) ? data : (int8_t)WL_SUCCESS;*/
    // This command is a very misleading no-op, see: https://github.com/adafruit/nina-fw/blob/d73fe315cc7f9148a0918490d3b75430c8444bf7/main/CommandHandler.cpp#L336-L345
    return WL_SUCCESS;
  }

  uint8_t Esp32Spi::get_scan_networks() {
    uint16_t ssid_list_num = 0;
    driver.send_command(SCAN_NETWORKS, nullptr, 0, (uint8_t *)network_ssid, &ssid_list_num, SpiDrv::RESPONSE_TYPE_NORMAL);
    return (uint8_t)ssid_list_num;
  }

  const char* Esp32Spi::get_ssid_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM) {
      return nullptr;
    }

    return network_ssid[network_item];
  }

  wl_enc_type Esp32Spi::get_enc_type_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM) {
      return ENC_TYPE_UNKNOWN;
    }

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&network_item)
    };

    uint8_t enc_type = 255;
    uint16_t data_len = 0;
    driver.send_command(GET_IDX_ENCT, params, PARAM_COUNT(params), &enc_type, &data_len);

    return (wl_enc_type)enc_type;
  }

  uint8_t* Esp32Spi::get_bssid_networks(uint8_t network_item, uint8_t* bssid_out) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM) {
      return nullptr;
    }

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&network_item)
    };

    uint16_t data_len = 0;   
    driver.send_command(GET_IDX_BSSID, params, 1, bssid_out, &data_len);

    return bssid_out;  
  }

  uint8_t Esp32Spi::get_channel_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM) {
      return 0;
    }

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&network_item)
    };

    uint8_t channel = 0;
    uint16_t data_len = 0;   
    driver.send_command(GET_IDX_CHANNEL, params, 1, &channel, &data_len);

    return channel;  
  }

  int32_t Esp32Spi::get_rssi_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM) {
      return 0;
    }

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&network_item)
    };

    int32_t  network_rssi = 0;
    uint16_t data_len = 0; 
    driver.send_command(GET_IDX_CHANNEL, params, 1, (uint8_t*)&network_rssi, &data_len);

    return network_rssi;
  }

  bool Esp32Spi::req_host_by_name(const std::string hostname) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&hostname)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    bool result = driver.send_command(REQ_HOST_BY_NAME, params, 1, &data, &data_len);  

    return result && data == WL_SUCCESS;
  }

  bool Esp32Spi::get_host_by_name(IPAddress& ip_out) {
    IPAddress dummy(0xFF,0xFF,0xFF,0xFF);
    uint8_t ip_addr[WL_IPV4_LENGTH];
    uint16_t data_len = 0;
    if(!driver.send_command(GET_HOST_BY_NAME, nullptr, 0, (uint8_t *)&ip_addr, &data_len)) {
      WARN("Error:GET_HOST_BY_NAME\n");
      return false;
    } else {
      ip_out = ip_addr;
      return (ip_out != dummy);
    }
  }

  bool Esp32Spi::get_host_by_name(const std::string hostname, IPAddress& ip_out) {
    if(req_host_by_name(hostname)) {
      return get_host_by_name(ip_out);
    }
    else {
      return false;
    }
  }

  const char* Esp32Spi::get_fw_version() {
    uint16_t data_len = 0;
    if(!driver.send_command(GET_FW_VERSION, nullptr, 0, (uint8_t*)fw_version, &data_len)){
      WARN("Error:GET_FW_VERSION\n");
    }
    return fw_version;
  }

  uint32_t Esp32Spi::get_time() {
    uint32_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(GET_TIME, nullptr, 0, (uint8_t*)&data, &data_len)) {
      WARN("Error:GET_TIME\n");
    }
    return data;
  }

  void Esp32Spi::set_power_mode(uint8_t mode) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&mode),
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    driver.send_command(SET_POWER_MODE, params, 1, &data, &data_len);
  }

  int8_t Esp32Spi::wifi_set_ap_network(const std::string ssid, uint8_t channel) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid),
      SpiDrv::build_param(&channel),
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_AP_NET, params, 2, &data, &data_len)) {
      WARN("Error:SET_AP_NET\n");
      data = WL_FAILURE;
    }

    return (data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
  }

  int8_t Esp32Spi::wifi_set_ap_passphrase(const std::string ssid, const std::string passphrase, uint8_t channel) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid),
      SpiDrv::build_param(&passphrase),
      SpiDrv::build_param(&channel)
    };

    int8_t data = WL_FAILURE;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_AP_PASSPHRASE, params, PARAM_COUNT(params), (uint8_t*)&data, &data_len)) {
      WARN("Error:SET_AP_PASSPHRASE\n");
    }
    return data;
  }

  int16_t Esp32Spi::ping(uint32_t ip_address, uint8_t ttl) {
    // Ping a remote address.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L919-L935
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ip_address),
      SpiDrv::build_param(&ttl)
    };

    int16_t data = WL_FAILURE;
    uint16_t data_len = 0;
    if(!driver.send_command(PING, params, PARAM_COUNT(params), (uint8_t*)&data, &data_len)) {
      WARN("Error:PING\n");
    }
    // Returns response time (presumably in milliseconds?)
    return data; 
  }

  void Esp32Spi::debug(uint8_t on) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&on)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_DEBUG, params, PARAM_COUNT(params), (uint8_t*)&data, &data_len)) {
      WARN("Error:SET_DEBUG\n");
    }
  }

  float Esp32Spi::get_temperature() {
    // Get the ESP32 die temperature?
    // This will *not* be ambient.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L239-L249
    float data = WL_FAILURE;
    uint16_t data_len = 0;
    if(!driver.send_command(GET_TEMPERATURE, nullptr, 0, (uint8_t *)&data, &data_len)) {
      WARN("Error:GET_TEMPERATURE\n");
    }
    return data;
  }

  void Esp32Spi::pin_mode(uint8_t pin, uint8_t mode) {
    // Set the mode of an ESP32 GPIO pin.
    // Uses the Arduino HAL "pinModes" internally.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L955-L967
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&pin),
      SpiDrv::build_param(&mode)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_PIN_MODE, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_PIN_MODE\n");
      data = WL_FAILURE;
    }
  }

  void Esp32Spi::digital_write(uint8_t pin, uint8_t value) {
    // Write a 0 or 1 to an ESP32 GPIO pin.
    // Uses the Arduino HAL "digitalWrite" internally.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L969-L981
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&pin),
      SpiDrv::build_param(&value),
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_DIGITAL_WRITE, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_DIGITAL_WRITE\n");
      data = WL_FAILURE; // ???
    }
  }

  void Esp32Spi::analog_write(uint8_t pin, uint8_t value) {
    // Write a 0 to 255 "analog" value to an ESP32 GPIO pin.
    // Uses the Arduino HAL "analogWrite" internally.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L983-L995
    SpiDrv::inParam params[] {
      SpiDrv::build_param(&pin),
      SpiDrv::build_param(&value)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_ANALOG_WRITE, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_ANALOG_WRITE\n");
      data = WL_FAILURE;
    }
  }

  bool Esp32Spi::digital_read(uint8_t pin) {
    // Read a 0 (LOW) or 1 (HIGH) from an ESP32 GPIO pin.
    // Uses the Arduino HAL "digitalRead" internally.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L997-L1008
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&pin)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_DIGITAL_READ, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_DIGITAL_READ\n");
    }

    return data == WL_SUCCESS;
  }

  uint16_t Esp32Spi::analog_read(uint8_t pin, uint8_t atten) {
    // Read an analog value from an ESP32 GPIO pin.
    // Uses the Arduino HAL "analogRead" internally.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L1010-L1023
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&pin),
      SpiDrv::build_param(&atten)
    };

    uint32_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_ANALOG_READ, params, PARAM_COUNT(params), (uint8_t*)&data, &data_len)) {
      WARN("Error:SET_ANALOG_READ\n");
    }

    return (uint16_t)data; // ESP only has a 12-bit ADC but returns a uint32
  }

  void Esp32Spi::start_server(uint16_t port, uint8_t sock, uint8_t protocol_mode) {
    port =__builtin_bswap16(port);

    SpiDrv::inParam params[] = {
        SpiDrv::build_param(&port),
        SpiDrv::build_param(&sock),
        SpiDrv::build_param(&protocol_mode),
    };
  
    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(START_SERVER_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:START_SERVER_TCP\n");
    }
  }

  void Esp32Spi::start_server(uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t protocol_mode) {
    port =__builtin_bswap16(port);

    SpiDrv::inParam params[] = {
        SpiDrv::build_param(&ip_address),
        SpiDrv::build_param(&port),
        SpiDrv::build_param(&sock),
        SpiDrv::build_param(&protocol_mode),
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(START_SERVER_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:START_SERVER_TCP\n");
    }
  }

  void Esp32Spi::start_client(uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t protocol_mode) {
    port = __builtin_bswap16(port); // Don't ask, I'll cry

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ip_address),
      SpiDrv::build_param(&port),
      SpiDrv::build_param(&sock),
      SpiDrv::build_param(&protocol_mode)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(START_CLIENT_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:START_CLIENT_TCP\n");
    }
  }

  void Esp32Spi::start_client(const std::string host, uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t protocol_mode) {
    port = __builtin_bswap16(port); // Don't ask, I'll cry

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&host),
      SpiDrv::build_param(&ip_address),
      SpiDrv::build_param(&port),
      SpiDrv::build_param(&sock),
      SpiDrv::build_param(&protocol_mode)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(START_CLIENT_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:START_CLIENT_TCP\n");
    } 
  }

  void Esp32Spi::stop_client(uint8_t sock) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock),
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(STOP_CLIENT_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:STOP_CLIENT_TCP\n");
    }
  }

  uint8_t Esp32Spi::get_server_state(uint8_t sock) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(GET_STATE_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:GET_STATE_TCP\n");
    }

    return data;
  }

  uint8_t Esp32Spi::get_client_state(uint8_t sock) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(GET_CLIENT_STATE_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:GET_CLIENT_STATE_TCP\n");
    }

    return data;
  }

  uint16_t Esp32Spi::avail_data(uint8_t sock) {
    if(!driver.available()) {
      return 0;
    }

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock),
    };

    uint16_t bytes_available = 0;
    uint16_t data_len = 0;
    driver.send_command(AVAIL_DATA_TCP, params, PARAM_COUNT(params), (uint8_t *)&bytes_available, &data_len);

    return bytes_available;
  }

  uint8_t Esp32Spi::avail_server(uint8_t sock) {
    // This function has a deliberate narrowing conversion from uint8_t to uint16_t
    // see: https://github.com/adafruit/nina-fw/blob/d73fe315cc7f9148a0918490d3b75430c8444bf7/main/CommandHandler.cpp#L437-L498
    if(!driver.available()) {
      return 255;
    }

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock),
    };

    uint16_t socket = 0;
    uint16_t data_len = 0;
    driver.send_command(AVAIL_DATA_TCP, params, PARAM_COUNT(params), (uint8_t *)&socket, &data_len);

    return socket;
  }

  bool Esp32Spi::get_data(uint8_t sock, uint8_t *data_out, bool peek) {
    // Peek or get a single byte of data
    // see: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L500-L529
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock),
      SpiDrv::build_param((uint8_t *)&peek),
    };
  
    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(GET_DATA_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:GET_DATA_TCP\n");
    }
    
    if(data_len != 0) {
      *data_out = data;
      return true;
    }
    return false;
  }

  bool Esp32Spi::get_data_buf(uint8_t sock, uint8_t *data_out, uint16_t *data_len_out) {
    // Get the data buffer for TCP/UDP/TLS clients.
    // see: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L870-L896
    SpiDrv::inParam params[] = {
      SpiDrv::build_param_buffer(&sock, 1),
      SpiDrv::build_param_buffer((uint8_t *)&data_len_out, 2),
    };
  
    if(!driver.send_command(GET_DATABUF_TCP, params, PARAM_COUNT(params), data_out, data_len_out, SpiDrv::RESPONSE_TYPE_DATA16)) {
      WARN("Error:GET_DATABUF_TCP\n");
    }

    if(*data_len_out != 0) {
      return true;
    }
    return false;
  }

  bool Esp32Spi::insert_data_buf(uint8_t sock, const uint8_t *data_in, uint16_t len) {
    // UDP only. Writes to the socket output buffer.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L898-L917
    SpiDrv::inParam params[] = {
      SpiDrv::build_param_buffer(&sock, 1),
      SpiDrv::build_param_buffer(data_in, len),
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(INSERT_DATABUF, params, PARAM_COUNT(params), &data, &data_len, SpiDrv::RESPONSE_TYPE_DATA8)) {
      WARN("Error:INSERT_DATABUF\n");
    }

    if(data_len != 0) {
      return (data == 1);
    }
    return false;
  }

  bool Esp32Spi::send_udp_data(uint8_t sock) {
    // UDP only. Writes the socket UDP end packet.
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L763-L777
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SEND_DATA_UDP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SEND_DATA_UDP\n");
    }

    // Returns 1 if udps[socket].endPacket() returns true
    return data_len && data == WL_SUCCESS;
  }

  uint16_t Esp32Spi::send_data(uint8_t sock, const uint8_t *data_in, uint16_t len) {
    // Send a data buffer over TCP (server/client) or TLS type sockets
    // See: https://github.com/adafruit/nina-fw/blob/104c48cb48e2a04c8a8009ef2db1b551414628a5/main/CommandHandler.cpp#L845-L868
    SpiDrv::inParam params[] = {
        SpiDrv::build_param_buffer(&sock, 1),
        SpiDrv::build_param_buffer(data_in, len),
    };

    uint16_t bytes_written = 0;
    uint16_t data_out_len = 0;
    if(!driver.send_command(SEND_DATA_TCP, params, PARAM_COUNT(params), (uint8_t *)&bytes_written, &data_out_len, SpiDrv::RESPONSE_TYPE_DATA8)) {
      WARN("Error:SEND_DATA_TCP\n");
    }

    // Returns the number of bytes written
    return bytes_written;
  }

  uint8_t Esp32Spi::check_data_sent(uint8_t sock) {
    const uint16_t TIMEOUT_DATA_SENT = 25;

    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&sock)
    };

    uint16_t timeout = 0;
    uint8_t data = 0;
    uint16_t data_len = 0;
    do {
      if(!driver.send_command(DATA_SENT_TCP, params, PARAM_COUNT(params), &data, &data_len)) {
        WARN("Error:DATA_SENT_TCP/n");
      }

      if(data)
        timeout = 0;
      else {
        ++timeout;
        sleep_ms(100);
      }

    } while((data == 0) && (timeout < TIMEOUT_DATA_SENT));

    return (timeout == TIMEOUT_DATA_SENT) ? 0 : 1;
  }

  uint8_t Esp32Spi::get_socket() {
    uint8_t socket = -1;
    uint16_t data_len = 0;
    if(!driver.send_command(GET_SOCKET, nullptr, 0, &socket, &data_len)) {
        WARN("Error:GET_SOCKET/n");
    }
    return socket;
  }

  void Esp32Spi::wifi_set_ent_identity(const std::string identity) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&identity)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(WPA2_ENT_SET_IDENTITY, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:WPA2_ENT_SET_IDENTITY\n");
    }
  }

  void Esp32Spi::wifi_set_ent_username(const std::string username) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&username)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(WPA2_ENT_SET_USERNAME, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:WPA2_ENT_SET_USERNAME\n");
    }
  }

  void Esp32Spi::wifi_set_ent_password(const std::string password) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&password)
    };

    uint8_t data;
    uint16_t data_len = 0;
    if (!driver.send_command(WPA2_ENT_SET_PASSWORD, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:WPA2_ENT_SET_PASSWORD\n");
    }
  }

  void Esp32Spi::wifi_set_ent_enable() {
    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(WPA2_ENT_ENABLE, nullptr, 0, &data, &data_len)) {
      WARN("Error:WPA2_ENT_ENABLE\n");
    }
  }

  void Esp32Spi::sleep_set_wake_pin(uint8_t wake_pin) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&wake_pin)
    };

    uint8_t data;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_WAKE_PIN, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_WAKE_PIN\n");
    }
  }

  void Esp32Spi::sleep_light() {
    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_LIGHT_SLEEP, nullptr, 0, &data, &data_len)) {
      WARN("Error:SET_LIGHT_SLEEP_CMD\n");
    } else {
      // Because we still have a very murky separation of concerns between esp32spi and spi_drv
      // we'll force the sleep state here, rather than have spi_drv understand this commmand
      driver.sleep_state = SpiDrv::LIGHT_SLEEP;
    }
  }
  
  void Esp32Spi::sleep_deep(uint8_t time) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&time)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if(!driver.send_command(SET_DEEP_SLEEP, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_DEEP_SLEEP\n");
    } else {
      // Because we still have a very murky separation of concerns between esp32spi and spi_drv
      // we'll force the sleep state here, rather than have spi_drv understand this commmand
      driver.sleep_state = SpiDrv::DEEP_SLEEP;
    }
  }
  
}