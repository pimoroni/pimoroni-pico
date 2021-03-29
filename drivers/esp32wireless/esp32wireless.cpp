#include "esp32wireless.hpp"

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
    GET_HOST_BY_NAME        = 0x35,         //FIXME No Matching Function yet (code exists)
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
    SET_CLIENT_CERT         = 0x40,         //BUG No Matching Function
    SET_CERT_KEY            = 0x41,         //BUG No Matching Function
    //NULL, NULL,
    SEND_DATA_TCP           = 0x44,
    GET_DATABUF_TCP         = 0x45,
    INSERT_DATABUF          = 0x46,
    //NULL, NULL, NULL,
    WPA2_ENT_SET_IDENTITY   = 0x4a,         //BUG No Matching Function //NOTE Exposed in CPy
    WPA2_ENT_SET_USERNAME   = 0x4b,         //BUG No Matching Function //NOTE Exposed in CPy
    WPA2_ENT_SET_PASSWORD   = 0x4c,         //BUG No Matching Function //NOTE Exposed in CPy
    WPA2_ENT_SET_CA_CERT    = 0x4d,         //BUG No Matching Function
    WPA2_ENT_SET_CERT_KEY   = 0x4e,         //BUG No Matching Function
    WPA2_ENT_ENABLE         = 0x4f,         //BUG No Matching Function //NOTE Exposed in CPy

    // 0x50 -> 0x5f
    SET_PIN_MODE            = 0x50,
    SET_DIGITAL_WRITE       = 0x51,
    SET_ANALOG_WRITE        = 0x52,
    SET_DIGITAL_READ        = 0x53,         //BUG No Matching Function //NOTE Exposed in CPy
    SET_ANALOG_READ         = 0x54,         //BUG No Matching Function //NOTE Exposed in CPy
  };

  bool WifiNINA::init() {
    driver.init();
    driver.reset();

    return true;
  }

  void WifiNINA::get_network_data(uint8_t *ip, uint8_t *mask, uint8_t *gwip) {
    SpiDrv::tParam params[SpiDrv::PARAM_NUMS_3] = { {0, ip},
                                                    {0, mask},
                                                    {0, gwip} };
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_IP_ADDR, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    driver.wait_response_params(GET_IP_ADDR, SpiDrv::PARAM_NUMS_3, params);
    driver.esp_deselect();
  }

  void WifiNINA::get_remote_data(uint8_t sock, uint8_t *ip, uint8_t *port) {
    SpiDrv::tParam params[SpiDrv::PARAM_NUMS_2] = { {0, ip},
                                                    {0, port} };
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_REMOTE_DATA, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    //Wait for reply
    driver.wait_response_params(GET_REMOTE_DATA, SpiDrv::PARAM_NUMS_2, params);
    driver.esp_deselect();
  }

  int8_t WifiNINA::wifi_set_network(const char* ssid, uint8_t ssid_len) {
  	driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_NET, SpiDrv::PARAM_NUMS_1);
    driver.send_param((const uint8_t*)ssid, ssid_len, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(5 + ssid_len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_NET, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();

    return (data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
  }

  int8_t WifiNINA::wifi_set_passphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_PASSPHRASE, SpiDrv::PARAM_NUMS_2);
    driver.send_param((const uint8_t*)ssid, ssid_len, SpiDrv::NO_LAST_PARAM);
    driver.send_param((const uint8_t*)passphrase, len, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(6 + ssid_len + len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    //Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_PASSPHRASE, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();

    return data;
  }

  int8_t WifiNINA::wifi_set_key(const char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len) {
	  driver.wait_for_esp_select();

    //Send Command
    driver.send_cmd(SET_KEY, SpiDrv::PARAM_NUMS_3);
    driver.send_param((uint8_t*)ssid, ssid_len, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&key_idx, KEY_IDX_LEN, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)key, len, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(8 + ssid_len + len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    //Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_KEY, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }    
    driver.esp_deselect();

    return data;
  }

  void WifiNINA::config(uint8_t valid_params, uint32_t local_ip, uint32_t gateway, uint32_t subnet) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_IP_CONFIG, SpiDrv::PARAM_NUMS_4);
    driver.send_param((uint8_t*)&valid_params, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)&local_ip, 4, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)&gateway, 4, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)&subnet, 4, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_IP_CONFIG, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();
  }

  void WifiNINA::set_dns(uint8_t valid_params, uint32_t dns_server1, uint32_t dns_server2) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_DNS_CONFIG, SpiDrv::PARAM_NUMS_3);
    driver.send_param((uint8_t*)&valid_params, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)&dns_server1, 4, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)&dns_server2, 4, SpiDrv::LAST_PARAM);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_DNS_CONFIG, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }

    driver.esp_deselect();
  }

  void WifiNINA::set_hostname(const char* hostname) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_HOSTNAME, SpiDrv::PARAM_NUMS_1);
    driver.send_param((uint8_t*)hostname, strlen(hostname), SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(5 + strlen(hostname));

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0 , data_len = 0;
    if(!driver.wait_response_cmd(SET_HOSTNAME, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();
  }

  int8_t WifiNINA::disconnect() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(DISCONNECT, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    int8_t result = driver.wait_response_cmd(DISCONNECT, SpiDrv::PARAM_NUMS_1, &data, &data_len);
    driver.esp_deselect();

    return result;
  }

  uint8_t WifiNINA::get_connection_status() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_CONN_STATUS, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = WL_FAILURE, data_len = 0;
    driver.wait_response_cmd(GET_CONN_STATUS, SpiDrv::PARAM_NUMS_1, &data, &data_len);

    driver.esp_deselect();

    return data;
  }

  uint8_t* WifiNINA::get_mac_address() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_MAC_ADDR, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);
    
    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data_len = 0;
    //TODO Decide what to do with _mac
    driver.wait_response_cmd(GET_MAC_ADDR, SpiDrv::PARAM_NUMS_1, _mac, &data_len);

    driver.esp_deselect();

    return _mac;
  }

  void WifiNINA::get_ip_address(uint8_t *ip_out) {
	  get_network_data(ip_out, _subnetMask, _gatewayIp);
    //TODO Sort this out
	  //_localIp = ip_out;
  }

  void WifiNINA::get_subnet_mask(uint8_t *mask_out) {
	  get_network_data(_localIp, _subnetMask, _gatewayIp);
    //TODO Sort this out
	  mask_out = _subnetMask;
  }

  void WifiNINA::get_gateway_ip(uint8_t *ip_out) {
  	get_network_data(_localIp, _subnetMask, _gatewayIp);
    //TODO Sort this out
    ip_out = _gatewayIp;
  }

  const char* WifiNINA::get_current_ssid() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_CURR_SSID, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    //TODO decide what to do with _ssid
    memset(_ssid, 0x00, sizeof(_ssid));

    // Wait for reply
    uint8_t data_len = 0;
    driver.wait_response_cmd(GET_CURR_SSID, SpiDrv::PARAM_NUMS_1, (uint8_t*)_ssid, &data_len);
    driver.esp_deselect();

    return _ssid;
  }

  uint8_t* WifiNINA::get_current_bssid() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_CURR_BSSID, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data_len = 0;
    //TODO decide what to do with _bssid
    driver.wait_response_cmd(GET_CURR_BSSID, SpiDrv::PARAM_NUMS_1, _bssid, &data_len);
    driver.esp_deselect();

    return _bssid;
  }

  int32_t WifiNINA::get_current_rssi() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_CURR_RSSI, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    int32_t rssi = 0;
    uint8_t data_len = 0;    
    driver.wait_response_cmd(GET_CURR_RSSI, SpiDrv::PARAM_NUMS_1, (uint8_t*)&rssi, &data_len);
    driver.esp_deselect();

    return rssi;
  }

  uint8_t WifiNINA::get_current_encryption_type() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_CURR_ENCT, SpiDrv::PARAM_NUMS_1);
    driver.send_byte_param(SpiDrv::DUMMY_DATA, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t enc_type = 0, data_len = 0;    
    driver.wait_response_cmd(GET_CURR_ENCT, SpiDrv::PARAM_NUMS_1, &enc_type, &data_len);
    driver.esp_deselect();

    return enc_type;
  }

  int8_t WifiNINA::start_scan_networks() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(START_SCAN_NETWORKS, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(START_SCAN_NETWORKS, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();

    return ((int8_t)data == WL_FAILURE) ? data : (int8_t)WL_SUCCESS;
  }

  uint8_t WifiNINA::get_scan_networks() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SCAN_NETWORKS, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t ssid_list_num = 0;
    //TODO _network_ssid something
    driver.wait_response(SCAN_NETWORKS, &ssid_list_num, (uint8_t**)network_ssid, WL_NETWORKS_LIST_MAXNUM);
    driver.esp_deselect();

    return ssid_list_num;
  }

  const char* WifiNINA::get_ssid_networks(uint8_t network_item) {
	  if(network_item >= WL_NETWORKS_LIST_MAXNUM)
		  return nullptr;

	  return network_ssid[network_item];
  }

  uint8_t WifiNINA::get_enc_type_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM)
		  return ENC_TYPE_UNKNOWN;

    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_IDX_ENCT, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&network_item, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t enc_type = 0, data_len = 0;    
    driver.wait_response_cmd(GET_IDX_ENCT, SpiDrv::PARAM_NUMS_1, &enc_type, &data_len);
    driver.esp_deselect();    

    return enc_type;
  }

  uint8_t* WifiNINA::get_bssid_networks(uint8_t network_item, uint8_t* bssid_out) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM)
      return nullptr;

    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_IDX_BSSID, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&network_item, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();    
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data_len = 0;
    driver.wait_response_cmd(GET_IDX_BSSID, SpiDrv::PARAM_NUMS_1, (uint8_t*)bssid_out, &data_len);
    driver.esp_deselect();

    return bssid_out;  
  }

  uint8_t WifiNINA::get_channel_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM)
      return 0;

    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_IDX_CHANNEL, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&network_item, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();    
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t channel = 0, data_len = 0;
    driver.wait_response_cmd(GET_IDX_CHANNEL, SpiDrv::PARAM_NUMS_1, &channel, &data_len);
    driver.esp_deselect();

    return channel;  
  }

  int32_t WifiNINA::get_rssi_networks(uint8_t network_item) {
    if(network_item >= WL_NETWORKS_LIST_MAXNUM)
  		return 0;

    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_IDX_RSSI, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&network_item, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();    
    driver.wait_for_esp_select();

    // Wait for reply
    int32_t	network_rssi = 0;
    uint8_t data_len = 0;
    driver.wait_response_cmd(GET_IDX_RSSI, SpiDrv::PARAM_NUMS_1, (uint8_t*)&network_rssi, &data_len);
    driver.esp_deselect();

	  return network_rssi;
  }

  uint8_t WifiNINA::req_host_by_name(const char* hostname) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(REQ_HOST_BY_NAME, SpiDrv::PARAM_NUMS_1);
    driver.send_param((uint8_t*)hostname, strlen(hostname), SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(5 + strlen(hostname));

    driver.esp_deselect();    
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    uint8_t result = driver.wait_response_cmd(REQ_HOST_BY_NAME, SpiDrv::PARAM_NUMS_1, &data, &data_len);
    driver.esp_deselect();    

    if(result) {
      result = (data == 1);
    }

    return result;
  }
/*
  //TODO GET_HOST_BY_NAME
  int WifiNINA::get_host_by_name(IPAddress& aResult);
  int WifiNINA::get_host_by_name(const char* aHostname, IPAddress& aResult); //Calls req_host_by_name and get_host_by_name
  //def get_host_by_name(self, hostname):   //Calls REQ_HOST_BY_NAME and GET_HOST_BY_NAME
*/
  const char* WifiNINA::get_fw_version() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_FW_VERSION, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();    
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data_len = 0;
    //TODO Decide what to do with fwVersion
    if(!driver.wait_response_cmd(GET_FW_VERSION, SpiDrv::PARAM_NUMS_1, (uint8_t*)fwVersion, &data_len)) {
      printf("error waitResponse\n");
    }
    driver.esp_deselect();

    return fwVersion;
  }

  uint32_t WifiNINA::get_time() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_TIME, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint32_t data = 0;
    uint8_t data_len = 0;
    if(!driver.wait_response_cmd(GET_TIME, SpiDrv::PARAM_NUMS_1, (uint8_t*)&data, &data_len)) {
      printf("error waitResponse\n");
    }
    driver.esp_deselect();

    return data;
  }

  void WifiNINA::set_power_mode(uint8_t mode) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_POWER_MODE, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&mode, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    driver.wait_response_cmd(SET_POWER_MODE, SpiDrv::PARAM_NUMS_1, &data, &data_len);
    driver.esp_deselect();
  }

  int8_t WifiNINA::wifi_set_ap_network(const char* ssid, uint8_t ssid_len, uint8_t channel) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_AP_NET, SpiDrv::PARAM_NUMS_2);
    driver.send_param((uint8_t*)ssid, ssid_len, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&channel, 1, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(3 + ssid_len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_AP_NET, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();

    return (data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
  }

  int8_t WifiNINA::wifi_set_ap_passphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len, uint8_t channel) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_AP_PASSPHRASE, SpiDrv::PARAM_NUMS_3);
    driver.send_param((uint8_t*)ssid, ssid_len, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)passphrase, len, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&channel, 1, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(4 + ssid_len + len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_AP_PASSPHRASE, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();

    return data;
  }

  int16_t WifiNINA::ping(uint32_t ip_address, uint8_t ttl) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(PING, SpiDrv::PARAM_NUMS_2);
    driver.send_param((uint8_t*)&ip_address, sizeof(ip_address), SpiDrv::NO_LAST_PARAM);
    driver.send_param(&ttl, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint16_t data;
    uint8_t data_len = 0;
    if(!driver.wait_response_cmd(PING, SpiDrv::PARAM_NUMS_1, (uint8_t*)&data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_PING_ERROR;
    }
    driver.esp_deselect();

    return data;  
  }

  void WifiNINA::debug(uint8_t on) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_DEBUG, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&on, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    driver.wait_response_cmd(SET_DEBUG, SpiDrv::PARAM_NUMS_1, &data, &data_len);
    driver.esp_deselect();
  }

  float WifiNINA::get_temperature() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_TEMPERATURE, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    float data = 0;
    uint8_t data_len = 0;
    if(!driver.wait_response_cmd(GET_TEMPERATURE, SpiDrv::PARAM_NUMS_1, (uint8_t*)&data, &data_len)) {
      printf("error get_temperature\n");
    }
    driver.esp_deselect();

    return data;
  }

  void WifiNINA::pin_mode(uint8_t pin, uint8_t mode) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_PIN_MODE, SpiDrv::PARAM_NUMS_2);
    driver.send_param(&pin, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&mode, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_PIN_MODE, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();
  }

  void WifiNINA::digital_write(uint8_t pin, uint8_t value) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_DIGITAL_WRITE, SpiDrv::PARAM_NUMS_2);
    driver.send_param(&pin, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&value, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_DIGITAL_WRITE, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error digital_write\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();
  }

  void WifiNINA::analog_write(uint8_t pin, uint8_t value) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SET_ANALOG_WRITE, SpiDrv::PARAM_NUMS_2);
    driver.send_param(&pin, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&value, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(SET_ANALOG_WRITE, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error analog_write\n");
      data = WL_FAILURE;
    }
    driver.esp_deselect();
  }

  void WifiNINA::start_server(uint16_t port, uint8_t sock, uint8_t prot_mode) {
	  driver.wait_for_esp_select();
    
    // Send Command
    driver.send_cmd(START_SERVER_TCP, SpiDrv::PARAM_NUMS_3);
    driver.send_param(port, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&prot_mode, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(START_SERVER_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("No response to server start/n");
    }
    driver.esp_deselect();
  }

  void WifiNINA::start_server(uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t prot_mode) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(START_SERVER_TCP, SpiDrv::PARAM_NUMS_4);
    driver.send_param((uint8_t*)&ip_address, sizeof(ip_address), SpiDrv::NO_LAST_PARAM);
    driver.send_param(port, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&prot_mode, 1, SpiDrv::LAST_PARAM);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(START_SERVER_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();
  }

  void WifiNINA::start_client(uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t prot_mode) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(START_CLIENT_TCP, SpiDrv::PARAM_NUMS_4);
    driver.send_param((uint8_t*)&ip_address, sizeof(ip_address), SpiDrv::NO_LAST_PARAM);
    driver.send_param(port, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&prot_mode, 1, SpiDrv::LAST_PARAM);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(START_CLIENT_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();
  }

  void WifiNINA::start_client(const char* host, uint8_t host_len, uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t prot_mode) {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(START_CLIENT_TCP, SpiDrv::PARAM_NUMS_5);
    driver.send_param((uint8_t*)host, host_len, SpiDrv::NO_LAST_PARAM);
    driver.send_param((uint8_t*)&ip_address, sizeof(ip_address), SpiDrv::NO_LAST_PARAM);
    driver.send_param(port, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(&prot_mode, 1, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(17 + host_len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(START_CLIENT_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();  
  }

  void WifiNINA::stop_client(uint8_t sock) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(STOP_CLIENT_TCP, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(STOP_CLIENT_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();
  }

  uint8_t WifiNINA::get_server_state(uint8_t sock) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_STATE_TCP, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(GET_STATE_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();

    return data;
  }

  uint8_t WifiNINA::get_client_state(uint8_t sock) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_CLIENT_STATE_TCP, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_cmd(GET_CLIENT_STATE_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();

    return data;
  }

  uint16_t WifiNINA::avail_data(uint8_t sock) {
    if(!driver.available()) {
      return 0;
    }

	  driver.wait_for_esp_select();
    
    // Send Command
    driver.send_cmd(AVAIL_DATA_TCP, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint16_t len = 0;
    uint8_t data_len = 0;
    driver.wait_response_cmd(AVAIL_DATA_TCP, SpiDrv::PARAM_NUMS_1, (uint8_t*)&len,  &data_len);
    driver.esp_deselect();

    return len;
  }


  uint8_t WifiNINA::avail_server(uint8_t sock) {
    if(!driver.available()) {
      return 255;
    }

    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(AVAIL_DATA_TCP, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint16_t socket = 0;
    uint8_t data_len = 0;
    driver.wait_response_cmd(AVAIL_DATA_TCP, SpiDrv::PARAM_NUMS_1, (uint8_t*)&socket,  &data_len);
    driver.esp_deselect();

    return socket;
  }

  bool WifiNINA::get_data(uint8_t sock, uint8_t *data_out, uint8_t peek) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_DATA_TCP, SpiDrv::PARAM_NUMS_2);
    driver.send_param(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_param(peek, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_data8(GET_DATA_TCP, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();
    
    if(data_len != 0) {
      *data_out = data;
      return true;
    }
    return false;
  }

  bool WifiNINA::get_data_buf(uint8_t sock, uint8_t *data_out, uint16_t *data_len_out) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_DATABUF_TCP, SpiDrv::PARAM_NUMS_2);
    driver.send_buffer(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_buffer((uint8_t *)data_len_out, sizeof(*data_len_out), SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    if(!driver.wait_response_data16(GET_DATABUF_TCP, data_out, data_len_out)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();

    if(*data_len_out != 0) {
      return true;
    }
    return false;
  }

  bool WifiNINA::insert_data_buf(uint8_t sock, const uint8_t *data_in, uint16_t len) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(INSERT_DATABUF, SpiDrv::PARAM_NUMS_2);
    driver.send_buffer(&sock, 1, SpiDrv::NO_LAST_PARAM);
    driver.send_buffer((uint8_t *)data_in, len, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(9 + len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_data8(INSERT_DATABUF, &data, &data_len)) {
      printf("error waitResponse");
    }
    driver.esp_deselect();

    if(data_len != 0) {
      return (data == 1);
    }
    return false;
  }

  bool WifiNINA::send_udp_data(uint8_t sock) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SEND_DATA_UDP, SpiDrv::PARAM_NUMS_1);
    driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

    // Pad to multiple of 4
    driver.read_byte();
    driver.read_byte();

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = 0, data_len = 0;
    if(!driver.wait_response_data8(SEND_DATA_UDP, &data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();

    if(data_len != 0) {
      return (data == 1);
    }
    return false;
  }

  uint16_t WifiNINA::send_data(uint8_t sock, const uint8_t *data_in, uint16_t len) {
	  driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(SEND_DATA_TCP, SpiDrv::PARAM_NUMS_2);
    driver.send_buffer(&sock, sizeof(sock));
    driver.send_buffer((uint8_t *)data_in, len, SpiDrv::LAST_PARAM);
    driver.pad_to_multiple_of_4(9 + len);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint16_t data = 0;
    uint8_t data_len = 0;
    if(!driver.wait_response_data8(SEND_DATA_TCP, (uint8_t*)&data, &data_len)) {
      printf("error waitResponse/n");
    }
    driver.esp_deselect();

    return data;
  }

  uint8_t WifiNINA::check_data_sent(uint8_t sock) {
    const uint16_t TIMEOUT_DATA_SENT = 25;

    uint16_t timeout = 0;
    uint8_t data = 0, data_len = 0;
    do {
      driver.wait_for_esp_select();

      // Send Command
      driver.send_cmd(DATA_SENT_TCP, SpiDrv::PARAM_NUMS_1);
      driver.send_param(&sock, 1, SpiDrv::LAST_PARAM);

      // Pad to multiple of 4
      driver.read_byte();
      driver.read_byte();

      driver.esp_deselect();
      driver.wait_for_esp_select();

      // Wait for reply
      if(!driver.wait_response_cmd(DATA_SENT_TCP, SpiDrv::PARAM_NUMS_1, &data, &data_len)) {
        printf("error waitResponse isDataSent/n");
      }
      driver.esp_deselect();

      if(data)
        timeout = 0;
      else {
        ++timeout;
        sleep_ms(100);
      }

    } while((data==0) && (timeout < TIMEOUT_DATA_SENT));

    return (timeout == TIMEOUT_DATA_SENT) ? 0 : 1;
  }

  uint8_t WifiNINA::get_socket() {
    driver.wait_for_esp_select();

    // Send Command
    driver.send_cmd(GET_SOCKET, SpiDrv::PARAM_NUMS_0);

    driver.esp_deselect();
    driver.wait_for_esp_select();

    // Wait for reply
    uint8_t data = -1, data_len = 0;
    driver.wait_response_cmd(GET_SOCKET, SpiDrv::PARAM_NUMS_1, &data, &data_len);
    driver.esp_deselect();

    return data;
  }
}